#ifndef PROPSYNTAXANALYZER_H
#define PROPSYNTAXANALYZER_H
#include "proplexanalyzer.h"
#include "proptreebuilder.h"

class PROPSyntaxAnalyzer
{
protected:
    PROPLexAnalyzer *_pLA;
    bool _isCompiled = false;
private:
    virtual bool Compile() = 0;
public:
    PROPTreeBuilder _treeBuilder;
    PROPSyntaxAnalyzer(PROPLexAnalyzer *pLa, PROPTreeBuilder treeBuilder) : _pLA(pLa), _treeBuilder(treeBuilder) {}
    virtual ~PROPSyntaxAnalyzer() {}
};
#endif // PROPSYNTAXANALYZER_H
