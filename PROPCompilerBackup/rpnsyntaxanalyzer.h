#ifndef RPNSYNTAXANALYZER_H
#define RPNSYNTAXANALYZER_H
#include "propsyntaxanalyzer.h"

class RPNSyntaxAnalyzer : public PROPSyntaxAnalyzer
{
public:
    RPNSyntaxAnalyzer(PROPLexAnalyzer *pLa, PROPTreeBuilder treeBuilder) : PROPSyntaxAnalyzer(pLa, treeBuilder) {}
    RPNSyntaxAnalyzer(PROPLexAnalyzer *pLa) : PROPSyntaxAnalyzer(pLa) {}
    virtual bool Compile();
};

#endif // RPNSYNTAXANALYZER_H
