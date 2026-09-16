#ifndef PROPLEXANALYZER_H
#define PROPLEXANALYZER_H

#include <QStack>
#include <QChar>
#include <QString>
#include <QFile>
#include <QTextStream>

class PROPLexAnalyzer
{
protected:
    QStack<QFile*> m_inputs;
    QStack<QChar> m_pendingChars;
    QTextStream m_Stream;
    int m_nLastPos;
public:
    /* Embedded struct to give lexical meaning to words from a file,
     * and be useful to the tree structure.*/
    struct LEXEM
    {
        enum Type{OPERATOR,ID,ENDOFF,ERROR,OPENBRACKET,CLOSEDBRAKET,EOL,COMMENT};
        Type m_type;
        QString m_sToken;
        int m_ntreeIndex;
    };
    LEXEM buildToken(LEXEM::Type,QString token);
    bool pushFile(const QString &fileName);
    void popFile();
    QString read();
    LEXEM getToken();

    PROPLexAnalyzer();
};

#endif // PROPLEXANALYZER_H
