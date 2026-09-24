#include <QDebug>
#include "proplexanalyzer.h"

PROPLexAnalyzer::PROPLexAnalyzer() : _lastPos(0)
{
}

PropLexem PROPLexAnalyzer::buildToken(PropLexemOp_Type type, const QString &token)
{
    PropLexem l(type, token);
    return l;
}

/**
 * @brief Opens a file and adds it to the input file stack.
 *
 * The file is opened in read-only text mode. If the file cannot be opened,
 * it is not added to the stack and the function returns false.
 *
 * @param fileName Path to the file to open.
 * @return true if the file was successfully opened and added to the stack;
 *         false otherwise.
 *
 * @note Ownership of the opened file is transferred to the input stack
 *       through a std::unique_ptr.
 */
bool PROPLexAnalyzer::pushFile(const QString &fileName)
{
    auto pFile = new QFile(fileName);
    if (pFile->open(QFile::ReadOnly | QFile::Text))
    {
        m_inputs.push(pFile);
        return true;
    }
    else
    {
        qDebug() << "Error while uploading the file" << Qt::endl;
        return false;
    }
}

/**
 * @brief Removes the top file from the input stack.
 *
 * The associated QFile is automatically destroyed when its
 * std::unique_ptr is removed from the stack.
 *
 * If the input stack is empty, no action is performed.
 */
void PROPLexAnalyzer::popFile()
{
    if (!m_inputs.empty())
    {
        auto pFile = m_inputs.pop();
        delete pFile;
    }
}

/**
 * @brief Reads all remaining content from the top input file.
 *
 * If the input stack is empty, an error message is returned.
 * Otherwise, all remaining content is read from the currently active file.
 *
 * @return The remaining content of the top input file, or an error message
 *         if the input stack is empty.
 */
QString PROPLexAnalyzer::read()
{
    if (m_inputs.empty())
        return "ERROR, file empty";
    QTextStream stream(m_inputs.top());
    return stream.readAll();
}

/**
 * @brief Appends a character to the current string.
 *
 * @param ch Character to append.
 * @return Reference to the current PROPLexAnalyzer object.
 */
void PropLexem::operator+=(QChar ch)
{
    _token += ch;
}

PropLexem PROPLexAnalyzer::getToken()
{
    enum LexPropState_Type
    {
        LEXPROP_FIRST_LETTER_STATE,
        LEXPROP_END_OP_STATE,
        LEXPROP_BEGIN_OP_STATE,
        LEXPROP_ID_STATE,
        LEXPROP_COMMENT_STATE
    };
    PropLexem L;
    LexPropState_Type behaviorStates = LEXPROP_FIRST_LETTER_STATE;
    _fileStream.setDevice(m_inputs.top());
    /* Initial set of the file to the last position after the previous run.*/
    _fileStream.seek(_lastPos);

    QChar takenChar;
    // Infinite loop taking char from the stream until reach to a LEXEM type.
    do
    {
        /*EOF reached case.*/
        if (_fileStream.atEnd())
        {
            switch (behaviorStates)
            {
            case 0:
                L.setType(ENDOFF);
                return L;
                break;
            case 1: //>
            case 2: //->
                L.setType(ERROR);
                return L;
                break;
            case 3:
                L.setType(ID);
                return L;
                break;
            }
        }

        /* Building pending ID case */
        buildPendingID(takenChar);

        /* Behavior states*/
        switch (behaviorStates)
        {
        /* First letter case.*/
        case LEXPROP_FIRST_LETTER_STATE:
            switch (takenChar.toLatin1())
            {
            case '#':
                behaviorStates = LEXPROP_COMMENT_STATE;
                break;
            case '&':
            case '|':
            case '!':
                L.setType(OPERATOR);
                L += takenChar;
                _lastPos = _fileStream.pos();
                return L;
                break;
            case '-':
                L += takenChar;
                behaviorStates = LEXPROP_END_OP_STATE;
                break;
            case '<':
                L += takenChar;
                behaviorStates = LEXPROP_BEGIN_OP_STATE;
                break;
            case '\n':
                L.setType(EOL);
                L += takenChar;
                _lastPos = _fileStream.pos();
                return L;
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case '(':
                L.setType(OPENBRACKET);
                L.setToken(takenChar);
                _lastPos = _fileStream.pos();
                return L;
                break;
            case ')':
                L.setType(CLOSEDBRAKET);
                L.setToken(takenChar);
                _lastPos = _fileStream.pos();
                return L;
                break;
            /* Any character that can be part of the ID name.*/
            default:
                if (takenChar.isLetter() || takenChar == '_')
                {
                    L += takenChar;
                    behaviorStates = LEXPROP_ID_STATE;
                }
                else
                {
                    L += takenChar;
                    L.setType(ERROR);
                    _lastPos = _fileStream.pos();
                    return L;
                }
                break;
            }
            break;
        /* Operator end found case.*/
        case LEXPROP_END_OP_STATE: // >
            L += takenChar;
            if ('>' == takenChar)
                L.setType(OPERATOR);
            else
                L.setType(ERROR);
            _lastPos = _fileStream.pos();
            return L;
            break;
        /* Operator begining found case.*/
        case LEXPROP_BEGIN_OP_STATE: // -
            L += takenChar;
            if ('-' == takenChar)
                behaviorStates = LEXPROP_END_OP_STATE;
            else
            {
                L.setType(ERROR);
                _lastPos = _fileStream.pos();
                return L;
            }
            break;
        case LEXPROP_ID_STATE: // ID
            if (takenChar.isLetterOrNumber() || takenChar == '_')
            {
                L += takenChar;
                behaviorStates = LEXPROP_ID_STATE;
            }
            else
            {
                L.setType(ID);
                _pendingChars.push(takenChar);
                _lastPos = _fileStream.pos();
                return L;
            }
            break;
        /* Comment found case.*/
        case LEXPROP_COMMENT_STATE: // Comments
            if (takenChar != '\n')
            {
                L += takenChar;
                behaviorStates = LEXPROP_COMMENT_STATE;
            }
            else
            {
                L.setType(COMMENT);
                _lastPos = _fileStream.pos();
                return L;
            }
            break;
        }
    } while (1);
}

void PROPLexAnalyzer::buildPendingID(QChar &takenChar)
{
    if (_pendingChars.size())
    {
        if (!_pendingChars.top().isSpace() || _pendingChars.top() == '\n')
            takenChar = _pendingChars.pop();
        else
            _fileStream >> takenChar;
    }
    /* First char found case.*/
    else
        _fileStream >> takenChar;
}
