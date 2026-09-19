#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QList>
#include <QtAlgorithms>

#include "seview.h"
#include "ui_seview.h"
#include "coverWidget.h"
#include "coverWidget.h"
#include "subjectWidget.h"
#include "conclusionsWidget.h"

#include "inferencekernel.h"
#include <QVector>


SEView::SEView(InferenceKernel *ik, QWidget *parent):
    inferenceKernel(ik),QWidget(parent),isKnowledgeExhausted(false),
    ui(new Ui::SEView)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/SEResources/logo.png"));
    setWindowTitle("Mech Lynx");

    inferenceKernel->resetWorkspace();
    topicConclusion.clear();

    setFixedSize(APPWIDTH,APPHEIGHT);

    pages=new QStackedWidget(this);
    backButton=new QPushButton;
    askButton=new QPushButton;
    nextButton=new QPushButton;

    backButton->setIconSize(QSize(BUTTONSIZE,BUTTONSIZE));
    askButton->setIconSize(QSize(BUTTONSIZE,BUTTONSIZE));
    nextButton->setIconSize(QSize(BUTTONSIZE,BUTTONSIZE));

    QHBoxLayout* buttonsLayout=new QHBoxLayout;
    buttonsLayout->addWidget(backButton);
    buttonsLayout->addWidget(askButton);
    buttonsLayout->addWidget(nextButton);

    QVBoxLayout* pagesLayout= new QVBoxLayout;
    pagesLayout->addWidget(pages);
    pagesLayout->addSpacing(5);
    pagesLayout->addLayout(buttonsLayout);

    setLayout(pagesLayout);
    //
    subjectPage=new SubjectWidget(this);
    coverPage=new CoverWidget(this);
    conclusionsPage=new ConclusionsWidget(this);

    pages->addWidget(subjectPage);
    pages->addWidget(coverPage);
    pages->addWidget(conclusionsPage);

    pages->setCurrentWidget(coverPage);

    QVector<int> tempRules=inferenceKernel->getResettedRules();
    std::sort(tempRules.begin(), tempRules.end());

    subjectPage->setRules(tempRules);
    //
    connect(inferenceKernel,SIGNAL(atomValueDemanded(int)),
            this,SLOT(onAtomDisplayed(int)));
    connect(backButton,SIGNAL(clicked()),
            this,SLOT(onBackClicked()));
    connect(askButton,SIGNAL(clicked()),
            this,SLOT(onAskClicked()));
    connect(nextButton,SIGNAL(clicked(bool)),this,SLOT(onNextClicked()));
    connect(inferenceKernel,SIGNAL(displayConclusions(QList<int>)),
            this,SLOT(onConclusionDisplayed(QList<int>)));
    connect(subjectPage,SIGNAL(subjectToggled(QSet<int>,bool)),
            this,SLOT(onToggledConclusions(QSet<int>,bool)));
    connect(inferenceKernel,SIGNAL(knowledgeExhausted()),
            this,SLOT(onKnowledgeBaseExhausted()));
    connect(conclusionsPage,SIGNAL(conclusionClicked(QString)),
            this,SLOT(onConclusionClicked(QString)));
}

QSet<int> SEView::getConclusionsByRule(QSet<int> rules)
{
    QList<int> temp(rules.begin(), rules.end());
    QVector<int> conclusions;
    conclusions.clear();
    QList<int> atomsToCheck;
    atomsToCheck.clear();

    for(unsigned int i=0; i<temp.size();++i){
        int currentIndex=temp.at(i);
        if(inferenceKernel->isAbleModusPonens(currentIndex))
            atomsToCheck+=inferenceKernel->getConsequentsValues(currentIndex);
        else if(inferenceKernel->isAbleModusTollens(currentIndex))
            atomsToCheck+=inferenceKernel->getAntecedentsValues(currentIndex);

        foreach(int entryAtom,atomsToCheck){
            if(inferenceKernel->doesConclusionExists(entryAtom))
                conclusions.append(entryAtom);
        }
        atomsToCheck.clear();
    }
    return QSet<int>(conclusions.begin(),conclusions.end());
}

void SEView::onAtomDisplayed(int atomEntry){
    int position=inferenceKernel->getNodeAt(atomEntry).getFirst();
    QString symbol=inferenceKernel->getSymbols().at(position);
    symbol.replace("_"," ");

    QMessageBox* msgBox=new QMessageBox(this);
    msgBox->setWindowTitle(QString("Preguntando sintomas: %1").arg(symbol));
    msgBox->setText(symbol+"?");
    QAbstractButton* yesButton=msgBox->addButton(tr("Yes"),
                                                 QMessageBox::YesRole);
    QAbstractButton* dontKnowButton=msgBox->addButton(tr("Don't know"),
                                                      QMessageBox::RejectRole);
    QAbstractButton* noButton=msgBox->addButton(tr("No"),
                                                QMessageBox::NoRole);

    msgBox->exec();

    while(1){
        if(msgBox->clickedButton()==yesButton){
            inferenceKernel->setValue2Atom(atomEntry,true);
            return;
        }
        else if(msgBox->clickedButton()== noButton){
            inferenceKernel->setValue2Atom(atomEntry,false);
            return;
        }
        else if(msgBox->clickedButton()== dontKnowButton)
            return;
    }
}

//Buttons Slots

void SEView::onBackClicked()
{
    if(pages->currentWidget()==coverPage)
        close();
    else if(pages->currentWidget()==subjectPage ||
            pages->currentWidget()==conclusionsPage)
        pages->setCurrentWidget(coverPage);
}

void SEView::onAskClicked()
{
    if(pages->currentWidget() == coverPage)
        QMessageBox::about(this,"About Mech Lynx","Copyright 2017");
    else if(pages->currentWidget()==subjectPage)
    {
        QSet<int> k(inferenceKernel->getConclusions().begin(),
                    inferenceKernel->getConclusions().end());
        inferenceKernel->backwardChainning(k);
    }
    else if(pages->currentWidget()==conclusionsPage){
        conclusionsPage->clearItems();
        pages->setCurrentWidget(subjectPage);
    }
}

void SEView::onNextClicked()
{
    if(pages->currentWidget()==coverPage)
        pages->setCurrentWidget(subjectPage);

    else if(pages->currentWidget()==subjectPage){
        if(topicConclusion.empty()){
            QMessageBox::warning(this, "The knowledge base is empty",
                                 "There is no rule to infer, please"
                                 "select another area to work");
            return;
        }
        inferenceKernel->resetWorkspace();
        inferenceKernel->backwardChainning(topicConclusion);
    }

    else if(pages->currentWidget()==conclusionsPage){
        if(isKnowledgeExhausted){
            isKnowledgeExhausted=false;
            pages->setCurrentWidget(coverPage);
        }
        else{
            conclusionsPage->clearItems();
            inferenceKernel->backwardChainning(topicConclusion);
        }
    }
}

void SEView::onConclusionDisplayed(QList<int> conclusions)
{
    QStringList conclusionString;
    foreach(int conclusion, conclusions){
        int position=inferenceKernel->getNodeAt(conclusion).getFirst();
        QString symbol=inferenceKernel->getSymbols().at(position);
        QString replaced=symbol.replace("_"," ");
        conclusionString.append(replaced);
    }

    foreach(QString string,conclusionString){
        conclusionsPage->onInsertConclusion(string);
    }

    QMessageBox* msgBox=new QMessageBox(this);
    msgBox->setWindowTitle("Showing possible failures");
    msgBox->setText(conclusionString.join("\n"));
    QAbstractButton* continueButton=msgBox->addButton(tr("Continue"),
                                                      QMessageBox::YesRole);
    QAbstractButton* stopButton=msgBox->addButton(tr("Stop Inference"),
                                                  QMessageBox::RejectRole);
    msgBox->exec();
    while(1){
        if(msgBox->clickedButton()==continueButton)
            inferenceKernel->setContinueInference(true);
        else if(msgBox->clickedButton()==stopButton){
            inferenceKernel->setContinueInference(false);
            pages->setCurrentWidget(conclusionsPage);
        }
        break;
    }
    return;
}

void SEView::onKnowledgeBaseExhausted()
{
    conclusionsPage->clearItems();

    QStringList stringList;

    foreach(int conclusion,inferenceKernel->getConclusionsByValueKeys()){
        int position=inferenceKernel->getNodeAt(conclusion).getFirst();
        QString symbol=inferenceKernel->getSymbols().at(position);
        QString temp=symbol.replace("_"," ");
        stringList.append(temp);
    }

    conclusionsPage->setItems(stringList);
    pages->setCurrentWidget(conclusionsPage);
    isKnowledgeExhausted=true;
}

void SEView::onToggledConclusions(QSet<int> subjectConclusion, bool checked)
{
    QSet<int> conclusions=getConclusionsByRule(subjectConclusion);

    if(checked)
        topicConclusion+=conclusions;
    else
        topicConclusion-=conclusions;
}

void SEView::onConclusionClicked(QString conclusionString)
{
    int entryConclusion=inferenceKernel->getMapSymbolEntry().value(conclusionString);
    QString explanation;
    explanation.clear();

    QVector<int> rules=inferenceKernel->getRules();
    auto antecedents=inferenceKernel->getAntecedents();
    auto consequents=inferenceKernel->getConsequents();

    foreach(int rule, rules){
        //Modus Ponens
        if(antecedents.values(rule).contains(entryConclusion)){
            foreach(int atomEntry,consequents.values(rule)){
                explanation+=inferenceKernel->getMapEntrySymbol().value(atomEntry);
                explanation+=" , ";
            }
        }
        else if(consequents.values(rule).contains(entryConclusion)){
            foreach(int atomEntry,antecedents.values(rule)){
                explanation+=inferenceKernel->getMapEntrySymbol().value(atomEntry);
                explanation+=" , ";
            }
        }
    }

    explanation.replace("_"," ");
    conclusionString.replace("_"," ");

    QMessageBox::information(this,QString("Explanation of %1").arg(conclusionString),
                             explanation);
}
//end ButtonsSlots

SEView::~SEView()
{
    delete ui;
}

//painting
void SEView::paintEvent(QPaintEvent *)
{
    if(pages->currentWidget()==coverPage){
        backButton->setIcon(
                    QIcon(":/SEResources/exitButton.jpg"));
        askButton->setIcon(
                    QIcon(":/SEResources/questionButton.jpg"));
        nextButton->setIcon(
                    QIcon(":/SEResources/nextButton.jpg"));
    }
}
//end painting
