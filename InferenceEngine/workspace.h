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
    QMultiMap<int,int> _mMapAntecedents;
    QMultiMap<int,int> _mMConsequents;
    QVector<int> _rules;
    QVector<int> _conclusions;
    QVector<int> _deniedAtoms;
    QMap<int,bool> _mRelevantAtoms;
    QMap<int,bool> _mAntecedentsValues;
    QMap<int,bool> _mConsequentsValues;
    QMap<int,bool> _mConclusionsValues;
    QMap<int,RuleType> _mRuleTypes;
    QVector<int> _inferredRules;
    QVector<int> _irrelevantRules;
    QVector<int> _askedAtoms;
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
