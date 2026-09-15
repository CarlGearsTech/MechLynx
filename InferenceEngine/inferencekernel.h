#ifndef INFERENCEKERNEL_H
#define INFERENCEKERNEL_H

#include "inferenceengine_global.h"
#include "workspace.h"

#include <QObject>
#include <QSet>
#include <QList>

class QuestionDialog;

class INFERENCEENGINESHARED_EXPORT InferenceKernel:public QObject
{
    Q_OBJECT

signals:
    void displayConclusions(QList<int> conclusions);
    void knowledgeExhausted();
    void atomValueDemanded(int atomEntry);

private:
    const PROPTreeBuilder* const m_pTB;
    Workspace* const m_pWS;
    bool m_bContinueInferenceProcess;

    QList<int> conclusionsToCheck;
    QList<int> atomsToCheck;
    QList<int> conclusionsToDisplay;

    void AddToConsequentsIfIsAbleMP(QVector<int> atomsToCheck);
    void addToAtomsToCheck_IfIsAbleModusPonens(int entry);
    void addToAtomsToCheckFromConsequentsIfableMP(int entry);
    void setAtomsToCheckFromAntecedentsIfAbleMT(int entry);
    void clearConclusionsToCheckDisplayAndAtomsToCheck();
    void logOnlyForFunctions(const char *funcName);
    void AppendAtomEntriesToConclusionBasedInConclusionandRelevantAtomsContainment();
    void EmitDisplayConclusionIfThereAreConclusionsToDisplay();
    void prepareConclusionToBeDisplayed(int entry);
    void AppendToConclusionToDisplayAndConclusionValuesIfEntryIsNotInConclusionValues();
    void emitAtomValueDemandedFromNotRelevantandNotAskedAntecedentAtom(int currentAntAtom);
    void processAntecedentsValueToEmitAtomDemandedValue(int currentRuleEntry);
    void passToInferredRule(int currentRuleEntry);
    void determineIfAntecedentPresent(int currentRuleEntry);
    void determineIfConsequentPresent(int currentRuleEntry);
    void inferAntecedent(int currentRuleEntry);
    void emitAtomValueDemandedFromNotRelevantNotAskedConsequentAtom(int currentConsqAtom);
    void processConsequentsValueToEmitDemandedValue(int currentRuleEntry);
    void inferAntecedents(int currentRuleEntry);
    void inferCurrentRule(int conclusionFound, int currentRuleEntry);
    void promptConclusionFound(int conclusionFound);
    void iterateRules(const QVector<int> &rules, int conclusionFound);
    void continueInferenceProcess();
    void stopByKnowledgeExhausted();
    int setNewInferredRuleAmount();

public:
    QList<int> getConsequentsValues(int index)const{return m_pWS->m_mmConsequents.values(index);}
    QList<int> getAntecedentsValues(int index)const{return m_pWS->m_mmAntecedents.values(index);}
    bool doesConclusionExists(int index);
    QList<int> getConclusionsByValueKeys()const{return m_pWS->m_mConclusionValues.keys();}
    QVector<int> getConclusions() const{return m_pWS->m_vConclusions;}
    QVector<PROPNode>  getTree() const {return m_pTB->getTree();}
    QVector<QString> getSymbols()const {return m_pTB->getSymbols();}
    QVector<int> getRules()const{return m_pWS->m_vRules;}
    auto getAntecedents()const{return m_pWS->m_mmAntecedents;}
    auto getConsequents()const{return m_pWS->m_mmConsequents;}
    QMap<int,QString> getMapEntrySymbol()const{return m_pTB->getMapEntrySymbol();}
    QMap<QString,int> getMapSymbolEntry()const{return m_pTB->getMapSymbolEntry();}
    InferenceKernel(PROPTreeBuilder* tb);
    ~InferenceKernel();
    bool askForValue(int entry);
    bool setValue2Atom(int entry,bool value);
    void propagate(int entry,bool value);
    void infer(int entry);
    int eval(int entry, Workspace::RuleType isConsequent);
    bool isCube(int entry);
    bool isClousure(int entry);
    bool isAbleModusPonens(int entry);
    bool isAbleModusTollens(int entry);
    bool areVerifiedNewConclusions(int entry);
    void setAtomsToCheckFromConsequentsIfAbleMP(int entry);
    void setContinueInference(bool s){m_bContinueInferenceProcess=s;}
    void forwardChainning(const QVector<int>& rules);
    void backwardChainning(QSet<int> k);
    QVector<int> getResettedRules()const;
    void inferConsequents(int currentRuleEntry);
    void resetWorkspace(){m_pWS->reset();}
};

#endif // INFERENCEKERNEL_H
