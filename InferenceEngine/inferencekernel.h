#ifndef INFERENCEKERNEL_H
#define INFERENCEKERNEL_H

#include <QObject>
#include <QSet>
#include <QList>
#include "inferenceengine_global.h"
#include "workspace.h"
#include "propsyntaxanalyzer.h"

class INFERENCEENGINESHARED_EXPORT InferenceKernel : public QObject
{
    Q_OBJECT
signals:
    void displayConclusions(QList<int> conclusions);
    void knowledgeExhausted();
    void atomValueDemanded(int atomEntry);

private:
    const PROPSyntaxAnalyzer *const _syntaxAnalyzer;
    Workspace *const _workspace;
    bool _isInferenceActive;

    QList<int> _conclusionsToProcess;
    QList<int> _atomsToProcess;
    QList<int> _conclusionsToDisplay;

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
    InferenceKernel(const PROPSyntaxAnalyzer *const pSyntaxAnalyzer);
    ~InferenceKernel();
    QList<int> getConsequentsValues(int index) const { return _workspace->_mMConsequents.values(index); }
    QList<int> getAntecedentsValues(int index) const { return _workspace->_mMapAntecedents.values(index); }
    bool doesConclusionExists(int index);
    QList<int> getConclusionsByValueKeys() const { return _workspace->_mConclusionsValues.keys(); }
    QVector<int> getConclusions() const { return _workspace->_conclusions; }
    PROPNode getNodeAt(qsizetype idx) const { return _syntaxAnalyzer->getNodeAt(idx); }
    QString getSymbolAt(qsizetype idx) const { return _syntaxAnalyzer->getSymbolAt(idx); }
    QVector<int> getRules() const { return _workspace->_rules; }
    auto getAntecedents() const { return _workspace->_mMapAntecedents; }
    auto getConsequents() const { return _workspace->_mMConsequents; }
    QString getSymbolFromEntry(int entry) const { return _syntaxAnalyzer->getSymbolFromEntry(entry); }
    int getEntryFromSymbol(const QString &str) const { return _syntaxAnalyzer->getEntryFromSymbol(str); }
    bool askForValue(int entry);
    bool setValue2Atom(int entry, bool value);
    void propagate(int entry, bool value);
    void infer(int entry);
    int eval(int entry, Workspace::RuleType isConsequent);
    bool isCube(int entry);
    bool isClousure(int entry);
    bool isAbleModusPonens(int entry);
    bool isAbleModusTollens(int entry);
    bool areVerifiedNewConclusions(int entry);
    void setAtomsToCheckFromConsequentsIfAbleMP(int entry);
    void setContinueInference(bool s) { _isInferenceActive = s; }
    void forwardChainning(const QVector<int> &rules);
    void backwardChainning(QSet<int> k);
    QVector<int> getResettedRules() const;
    void inferConsequents(int currentRuleEntry);
    void resetWorkspace() { _workspace->reset(); }
};

#endif // INFERENCEKERNEL_H
