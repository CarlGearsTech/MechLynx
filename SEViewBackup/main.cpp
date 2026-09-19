#include "seview.h"
#include <QApplication>
#include <QMessageBox>

#include "proplexanalyzer.h"
#include "proptreebuilder.h"
#include "propnode.h"
#include "propsyntaxanalyzer.h"
#include "workspace.h"
#include "inferencekernel.h"
#include "infixsyntaxanalyzer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PROPTreeBuilder tb;
    PROPLexAnalyzer la;
    InfixSyntaxAnalyzer iasa(&la,&tb);
    InferenceKernel ik(&tb);

    if(!la.pushFile(":/SEResources/KnowledgeBase")){
        QMessageBox::critical(NULL,"Knowledge Base was not opened correctly",
                              "Check the reference and/or file state",
                              QMessageBox::StandardButton::Ok);
        return 0;
    }
    if(!iasa.Compile()){
        QMessageBox::critical(NULL,"Knowledge Base was not opened correctly",
                              "Check the reference and/or file state",
                              QMessageBox::StandardButton::Ok);
        return 0;
    }
    SEView w(&ik);
    w.show();

    return a.exec();
}
