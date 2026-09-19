#ifndef INFIXSYNTAXANALYZER_H
#define INFIXSYNTAXANALYZER_H
#include "propsyntaxanalyzer.h"

class InfixSyntaxAnalyzer: public PROPSyntaxAnalyzer{
    void buildLeftToken(int& left,int& right,PROPLexAnalyzer::LEXEM& operation,
                        unsigned int& signal,
                        QStack<int>& builderStack,QStack<PROPLexAnalyzer::LEXEM>& tokenStack,
                        bool& isValidated)const;

public:

    InfixSyntaxAnalyzer(PROPLexAnalyzer* pLA, PROPTreeBuilder* pTB):
        PROPSyntaxAnalyzer(pLA,pTB){}
    bool Compile()const;

};
#endif // INFIXSYNTAXANALYZER_H
