#ifndef PROPSYNTAXANALYZER_H
#define PROPSYNTAXANALYZER_H
#include "proplexanalyzer.h"
#include "proptreebuilder.h"

class PROPSyntaxAnalyzer
{
protected:
    PROPLexAnalyzer *_pLA;
    PROPTreeBuilder _treeBuilder;
private:
public:
    virtual bool Compile() = 0;
    PROPSyntaxAnalyzer(PROPLexAnalyzer *pLa, PROPTreeBuilder treeBuilder) : _pLA(pLa), _treeBuilder(treeBuilder) {}
    PROPSyntaxAnalyzer(PROPLexAnalyzer *pLa) : _pLA(pLa) {}
    PROPTreeBuilder getTreeBuilder() const { return _treeBuilder; }
    [[nodiscard]] QString getSymbolAt(qsizetype idx) const { return _treeBuilder.getSymbolAt(idx);}
    [[nodiscard]] PROPNode getNodeAt(qsizetype idx) const { return _treeBuilder.getNodeAt(idx);}
    [[nodiscard]] qsizetype getTreeSize() const { return _treeBuilder.getTreeSize();}
    [[nodiscard]] QString getSymbolFromEntry(int entry) const { return _treeBuilder.getSymbolFromEntry(entry);}
    [[nodiscard]] int getEntryFromSymbol(const QString &str) const { return _treeBuilder.getEntryFromSymbol(str);}
    virtual ~PROPSyntaxAnalyzer() {}
};
#endif // PROPSYNTAXANALYZER_H
