#ifndef PROPSYNTAXANALYZER_H
#define PROPSYNTAXANALYZER_H
#include "proplexanalyzer.h"
#include "proptreebuilder.h"

class PROPSyntaxAnalyzer
{
protected:
    PROPLexAnalyzer *_pLA;
    PROPTreeBuilder *_pTB;
private:
    virtual bool Compile() const = 0;
public:
    PROPSyntaxAnalyzer(PROPLexAnalyzer *pLa, PROPTreeBuilder *pTB) : _pLA(pLa), _pTB(pTB) {}
    virtual ~PROPSyntaxAnalyzer() {}
};

#endif // PROPSYNTAXANALYZER_H
