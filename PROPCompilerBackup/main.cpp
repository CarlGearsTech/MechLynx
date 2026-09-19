#include "proptreebuilder.h"
#include "proplexanalyzer.h"
#include "infixsyntaxanalyzer.h"

#include <iostream>
#include<QTextStream>

/* Target for testing PROPCompiler*/
int main(int argc, char *argv[])
{
    QTextStream out(stdout);
    PROPTreeBuilder tb;
    PROPLexAnalyzer la;
    InfixSyntaxAnalyzer IS(&la,&tb);

    (void)argc;
    (void)argv;

    if(la.pushFile(":/Resources/KnowledgeBase")){
        //PROPLexAnalyzer::LEXEM l=la.getToken();
        if(IS.Compile())
            out<<"Success in the compilation";
    }
    return 0;
}
