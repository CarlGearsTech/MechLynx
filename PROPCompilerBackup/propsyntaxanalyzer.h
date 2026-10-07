#ifndef PROPSYNTAXANALYZER_H
#define PROPSYNTAXANALYZER_H
#include "proplexanalyzer.h"
#include "proptreebuilder.h"

class PROPSyntaxAnalyzer
{
protected:
    PROPLexAnalyzer *_pLA;
private:
    virtual bool Compile() = 0;
public:
    PROPTreeBuilder _treeBuilder;
    PROPSyntaxAnalyzer(PROPLexAnalyzer *pLa, PROPTreeBuilder treeBuilder) : _pLA(pLa), _treeBuilder(treeBuilder) {}
    PROPSyntaxAnalyzer(PROPLexAnalyzer *pLa) : _pLA(pLa){}
    PROPTreeBuilder getTreeBuilder()const {return _treeBuilder;}
    virtual ~PROPSyntaxAnalyzer() {}
};
#endif // PROPSYNTAXANALYZER_H
