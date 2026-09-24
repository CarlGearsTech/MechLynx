#ifndef PROPLEXANALYZER_H
#define PROPLEXANALYZER_H

#include <QStack>
#include <QChar>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <memory>

enum PropLexemOp_Type
{
    OPERATOR,
    ID,
    ENDOFF,
    ERROR,
    OPENBRACKET,
    CLOSEDBRAKET,
    EOL,
    COMMENT
};

/**
 * @brief Embedded structure that assigns lexical meaning to words read from a file
 *        and provides information used by the tree structure.
 */
class PropLexem
{
public:
    PropLexem() = default;
    PropLexem(PropLexemOp_Type type, const QString& token): _type(type), _token(token), _treeIdx() {}
    void setType(PropLexemOp_Type type) {_type = type;}
    PropLexemOp_Type getType()const {return _type;}
    void operator+=(QChar ch);
    void setToken(const QString& token) {_token = token;}
    QString getToken()const {return _token;}
    void setTreeIdx(int idx) {_treeIdx = idx;}
    int getTreeIdx()const {return _treeIdx;}
private:
    PropLexemOp_Type _type;
    QString _token;
    int _treeIdx;
};

class PROPLexAnalyzer
{
protected:
    QStack<QFile*> m_inputs;
    QStack<QChar> m_pendingChars;
    QTextStream m_Stream;
    int m_nLastPos;
public:
    PropLexem buildToken(PropLexemOp_Type type, const QString &token);
    bool pushFile(const QString &fileName);
    void popFile();
    QString read();
    PropLexem getToken();
    PROPLexAnalyzer(const PROPLexAnalyzer& pLA) = delete;
    PROPLexAnalyzer& operator=(const PROPLexAnalyzer& pLA) = delete;
    PROPLexAnalyzer(PROPLexAnalyzer&& pLA) noexcept = default;
    PROPLexAnalyzer& operator=(PROPLexAnalyzer&& pLA) noexcept = default;
    PROPLexAnalyzer();
};
#endif // PROPLEXANALYZER_H
