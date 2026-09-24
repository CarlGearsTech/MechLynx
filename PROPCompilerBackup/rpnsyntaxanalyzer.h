#ifndef RPNSYNTAXANALYZER_H
#define RPNSYNTAXANALYZER_H
#include "propsyntaxanalyzer.h"

class RPNSyntaxAnalyzer : public PROPSyntaxAnalyzer
{
public:
    RPNSyntaxAnalyzer(PROPLexAnalyzer *pLa, PROPTreeBuilder *pTB) : PROPSyntaxAnalyzer(pLa, pTB) {}
    virtual bool Compile() const;
};

#endif // RPNSYNTAXANALYZER_H
