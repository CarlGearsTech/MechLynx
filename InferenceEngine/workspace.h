#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QMap>
#include <QVector>
#include <QMultiMap>
#include "proptreebuilder.h"

class Workspace
{
public:
    friend class InferenceKernel;
    enum RuleType{ModusTollens,ModusPonens,NoType};
private:
    Workspace(const PROPTreeBuilder& treeBuilder):_treeBuilder(treeBuilder){}
    PROPTreeBuilder _treeBuilder;
    QMultiMap<int,int> m_mmAntecedents;
    QMultiMap<int,int> m_mmConsequents;
    QVector<int> m_vRules;
    QVector<int> m_vConclusions;
    QVector<int> m_vDeniedAtoms;
    QMap<int,bool> m_mRelevantAtoms;
    QMap<int,bool> m_mAntecedentsValue;
    QMap<int,bool> m_mConsequentsValue;
    QVector<int> m_vInferredRules;
    QMap<int,RuleType> m_mRuleTypes;
    QVector<int> m_vIrrelevantRules;
    QVector<int> m_vAskedAtoms;
    QMap<int,bool> m_mConclusionValues;
    void InsertAntecedentOrConsequent(int rule, int entry,bool isConsequent);
public:
    void reset();
    void resetRules();
    void resetAntecedentsAndConsequents();
    void pickAtom(int entry,int rule,bool isConsequent);
    void resetDeniedAtoms();
    void resetConclusions();
    ~Workspace(){}
};

#endif // WORKSPACE_H
