#ifndef PROPSYNTAXANALYZER_H
#define PROPSYNTAXANALYZER_H
#include "proplexanalyzer.h"
#include "proptreebuilder.h"

class PROPSyntaxAnalyzer
{
protected:
    PROPLexAnalyzer* m_pLA;
    PROPTreeBuilder* m_pTB;
private:
    virtual bool Compile()const=0;
public:
    PROPSyntaxAnalyzer(PROPLexAnalyzer* pLa, PROPTreeBuilder* pTB):
        m_pLA(pLa),m_pTB(pTB){}
    virtual ~PROPSyntaxAnalyzer(){}
};

#endif // PROPSYNTAXANALYZER_H
