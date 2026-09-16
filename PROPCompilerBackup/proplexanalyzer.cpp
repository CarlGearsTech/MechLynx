#include <QDebug>
#include "proplexanalyzer.h"


PROPLexAnalyzer::PROPLexAnalyzer():m_nLastPos(0)
{
    m_inputs.clear();
}

PROPLexAnalyzer::LEXEM PROPLexAnalyzer::buildToken(LEXEM::Type type, QString token)
{
    PROPLexAnalyzer::LEXEM l;
    l.m_type=type;
    l.m_sToken=token;
    return l;
}

bool PROPLexAnalyzer::pushFile(const QString& fileName)
{
    QFile* file = new QFile(fileName);
    if(!file->exists())
        qDebug()<<"File does not exits";
    if(file->open(QFile::ReadOnly | QFile::Text))
    {
        m_inputs.push(file);
        return true;
    }
    qDebug()<<"Error while uploading the file";
    return false;
}

void PROPLexAnalyzer::popFile(){
    if(m_inputs.empty())
        return;
    QFile* file=m_inputs.top();
    m_inputs.pop()->close();
    delete file;
}

QString PROPLexAnalyzer::read(){
    if(m_inputs.empty())
        return QString("ERROR, file empty");
    QTextStream stream(m_inputs.top());
    return stream.readAll();
}

PROPLexAnalyzer::LEXEM PROPLexAnalyzer::getToken(){
    LEXEM L;
    int nStates=0;

    m_Stream.setDevice(m_inputs.top());
    m_Stream.seek(m_nLastPos);

    QChar takenChar;
    //Infinite loop taking char from the stream until reach to a LEXEM type.
    do{

        //Handling cases where it reached the End of File
        if(m_Stream.atEnd()){
            switch(nStates){
            case 0:
                L.m_type=LEXEM::ENDOFF;
                return L;
                break;
            case 1: //>
            case 2: //->
                L.m_type=LEXEM::ERROR;
                return L;
                break;
            case 3:
                L.m_type=LEXEM::ID;
                return L;
                break;
            }
        }

        //Handling pending Chars from the stream to ID building
        if(m_pendingChars.size()){
            if(!m_pendingChars.top().isSpace() || m_pendingChars.top()=='\n')
                takenChar=m_pendingChars.pop();
            else
                m_Stream>>takenChar;
        }
        else
            m_Stream>>takenChar;

        //Defines the different behaviour based in states:
        //Cases defines what means every state.
        switch (nStates) {
        case 0://First Letter found
            switch (takenChar.toLatin1()) {
            case '#':
                nStates=4;
                break;
            case '&':
            case '|':
            case '!':
                L.m_type=LEXEM::OPERATOR;
                L.m_sToken+=takenChar;
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            case '-':
                L.m_sToken+=takenChar;
                nStates=1;
                break;
            case '<':
                L.m_sToken+=takenChar;
                nStates=2;
                break;
            case '\n':
                L.m_type=LEXEM::EOL;
                L.m_sToken+=takenChar;
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case'(':
                L.m_type=LEXEM::OPENBRACKET;
                L.m_sToken=takenChar;
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            case ')':
                L.m_type=LEXEM::CLOSEDBRAKET;
                L.m_sToken=takenChar;
                m_nLastPos=m_Stream.pos();
                return L;
                break;
            default:
                if(takenChar.isLetter() || takenChar=='_'){
                    L.m_sToken+=takenChar;
                    nStates=3;
                }
                else{
                    L.m_sToken+=takenChar;
                    L.m_type=LEXEM::ERROR;
                    m_nLastPos=m_Stream.pos();
                    return L;
                }
                break;
            }
            break;
        case 1: //>
            L.m_sToken+=takenChar;
            if('>'==takenChar)
                L.m_type=LEXEM::OPERATOR;
            else
                L.m_type=LEXEM::ERROR;
            m_nLastPos=m_Stream.pos();
            return L;
            break;

        case 2:// -
            L.m_sToken+=takenChar;
            if('-' == takenChar)
                nStates=1;
            else{
                L.m_type=LEXEM::ERROR;
                m_nLastPos=m_Stream.pos();
                return L;
            }
            break;

        case 3: //ID
            if(takenChar.isLetterOrNumber() || takenChar=='_'){
                L.m_sToken+=takenChar;
                nStates=3;
            }
            else{
                L.m_type=LEXEM::ID;
                m_pendingChars.push(takenChar);
                m_nLastPos=m_Stream.pos();
                return L;
            }
            break;

        case 4:// Comments
            if(takenChar != '\n'){
                L.m_sToken+=takenChar;
                nStates=4;
            }
            else{
                L.m_type=LEXEM::COMMENT;
                m_nLastPos=m_Stream.pos();
                return L;
            }
            break;
        }

    }while(1);

}
