#include <QDebug>
#include "proplexanalyzer.h"

PROPLexAnalyzer::PROPLexAnalyzer():m_nLastPos(0)
{
    
}

PropLexem PROPLexAnalyzer::buildToken(PropLexemOp_Type type,const QString& token)
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
bool PROPLexAnalyzer::pushFile(const QString& fileName)
{
    auto pFile = new QFile(fileName);
    if(pFile->open(QFile::ReadOnly | QFile::Text))
    {
        m_inputs.push(pFile);
        return true;
    }
    else
    {
        qDebug()<<"Error while uploading the file"<< Qt::endl;
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
    if(!m_inputs.empty())
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
    if(m_inputs.empty())
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
    _token+= ch;
}

PropLexem PROPLexAnalyzer::getToken()
{
    PropLexem L;
    int nStates=0;

    m_Stream.setDevice(m_inputs.top());
    m_Stream.seek(m_nLastPos);

    QChar takenChar;
    //Infinite loop taking char from the stream until reach to a LEXEM type.
    do
    {
        //Handling cases where it reached the End of File
        if(m_Stream.atEnd()){
            switch(nStates){
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

        //Handling pending Chars from the stream to ID building
        if(m_pendingChars.size())
        {
            if(!m_pendingChars.top().isSpace() || m_pendingChars.top()=='\n')
                takenChar=m_pendingChars.pop();
            else
                m_Stream>>takenChar;
        }
        else
            m_Stream>>takenChar;

        //Defines the different behaviour based in states:
        //Cases defines what means every state.
        switch (nStates) 
        {
        case 0://First Letter found
            switch (takenChar.toLatin1()) 
            {
            case '#':
                nStates=4;
                break;
            case '&':
            case '|':
            case '!':
                L.setType(OPERATOR);
                L+=takenChar;
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            case '-':
                L+=takenChar;
                nStates=1;
                break;
            case '<':
                L+=takenChar;
                nStates=2;
                break;
            case '\n':
                L.setType(EOL);
                L+=takenChar;
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case'(':
                L.setType(OPENBRACKET);
                L.setToken(takenChar);
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            case ')':
                L.setType(CLOSEDBRAKET);
                L.setToken(takenChar);
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            default:
                if(takenChar.isLetter() || takenChar=='_'){
                    L+=takenChar;
                    nStates=3;
                }
                else{
                    L+=takenChar;
                    L.setType(ERROR);
                    m_nLastPos=m_Stream.pos();
                    return L;
                }
                break;
            }
            break;
        case 1: //>
            L+=takenChar;
            if('>'==takenChar)
                L.setType(OPERATOR);
            else
                L.setType(ERROR);
            m_nLastPos=m_Stream.pos();
            return L;
            break;

        case 2:// -
            L+=takenChar;
            if('-' == takenChar)
                nStates=1;
            else{
                L.setType(ERROR);
                m_nLastPos=m_Stream.pos();
                return L;
            }
            break;

        case 3: //ID
            if(takenChar.isLetterOrNumber() || takenChar=='_'){
                L+=takenChar;
                nStates=3;
            }
            else{
                L.setType(ID);
                m_pendingChars.push(takenChar);
                m_nLastPos=m_Stream.pos();
                return L;
            }
            break;

        case 4:// Comments
            if(takenChar != '\n'){
                L+=takenChar;
                nStates=4;
            }
            else{
                L.setType(COMMENT);
                m_nLastPos=m_Stream.pos();
                return L;
            }
            break;
        }
    }while(1);
}


