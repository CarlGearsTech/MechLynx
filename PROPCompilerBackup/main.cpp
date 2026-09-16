#include "propnode.h"
#include "proptreebuilder.h"
#include "proplexanalyzer.h"
#include "infixsyntaxanalyzer.h"

#include <iostream>
#include<QTextStream>

int main(int argc, char *argv[])
{
    QTextStream out(stdout);
    PROPTreeBuilder tb;
    PROPLexAnalyzer la;

    InfixSyntaxAnalyzer IS(&la,&tb);

    if(la.pushFile(":/Resources/KnowledgeBase")){
        //PROPLexAnalyzer::LEXEM l=la.getToken();
        if(IS.Compile())
            out<<"Success in the compilation";
    }
    return 0;
}
