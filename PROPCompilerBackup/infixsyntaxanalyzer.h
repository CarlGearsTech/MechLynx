#ifndef INFIXSYNTAXANALYZER_H
#define INFIXSYNTAXANALYZER_H
#include "propsyntaxanalyzer.h"

class InfixSyntaxAnalyzer : public PROPSyntaxAnalyzer
{
private:
    void buildLeftToken(int &left,
                        int &right,
                        PropLexem &operation,
                        unsigned int &signal,
                        QStack<int> &builderStack,
                        QStack<PropLexem> &tokenStack,
                        bool &isValidated);
    bool determineResultCompilation(unsigned int l_nOpenB, unsigned int l_nClosedB, unsigned int &signal, QStack<int> &l_builderStack, QStack<PropLexem> &l_tokenStack, bool &retFlag);
public:
    InfixSyntaxAnalyzer(PROPLexAnalyzer *pLA, PROPTreeBuilder treeBuilder) : PROPSyntaxAnalyzer(pLA, treeBuilder) {}
    [[nodiscard]] bool Compile();
    [[nodiscard]] QString getSymbolAt(qsizetype idx) const {return _treeBuilder.getSymbolAt(idx);}
    [[nodiscard]] PROPNode getNodeAt(qsizetype idx) const {return _treeBuilder.getNodeAt(idx);}
    [[nodiscard]] qsizetype getTreeSize() const {return _treeBuilder.getTreeSize();}
    [[nodiscard]] QString getSymbolFromEntry(int entry) const {return _treeBuilder.getSymbolFromEntry(entry);}
    [[nodiscard]] int getEntryFromSymbol(const QString &str) const {return _treeBuilder.getEntryFromSymbol(str);}
};
#endif // INFIXSYNTAXANALYZER_H
