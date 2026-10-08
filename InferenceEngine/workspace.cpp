#include "workspace.h"
#include "proptreebuilder.h"


void Workspace::InsertAntecedentOrConsequent(int rule, int entry, bool isConsequent)
{
    if(!isConsequent)
        _mMapAntecedents.insert(rule,entry);
    else
        _mMConsequents.insert(rule,entry);
}

void Workspace::reset(){
    resetRules();
    resetAntecedentsAndConsequents();
    resetConclusions();
    resetDeniedAtoms();
    resetRules();
    _mRuleTypes.clear();
    _mAntecedentsValues.clear();
    _mConsequentsValues.clear();
    _askedAtoms.clear();
    _inferredRules.clear();
    _irrelevantRules.clear();
    _mConclusionsValues.clear();
    _mRelevantAtoms.clear();
}

void Workspace::resetRules()
{
    _rules.clear();
    for(qsizetype i=0; i < _treeBuilder.getTreeSize() ; ++i)
    {
        const auto& node = _treeBuilder.getNodeAt(i);
        if(node.getType() == IF)
            _rules.append(i);
    }
}

void Workspace::resetAntecedentsAndConsequents(){
    _mMapAntecedents.clear();
    _mConsequentsValues.clear();

    for(int i=0, lentry=0,rentry=0;i<_rules.size();++i){
        lentry=_treeBuilder.getNodeAt(_rules.at(i)).getFirst();
        pickAtom(lentry,_rules.at(i),0);

        rentry=_treeBuilder.getNodeAt(_rules.at(i)).getSecond();
        pickAtom(rentry,_rules.at(i),1);
    }
}

void Workspace::pickAtom(int entry, int rule, bool isConsequent){
    //Reach a atom type
    //Check operationst
    if(_treeBuilder.getNodeAt(entry).getType() == ATOM){
        InsertAntecedentOrConsequent(rule,entry,isConsequent);
        return;
    }

    //Operation type and verify its first and second
    //Check left part of the operation
    int first=_treeBuilder.getNodeAt(entry).getFirst();
    //Operation Type
    if(_treeBuilder.getNodeAt(first).getType() != ATOM){
        if(!isConsequent)
            pickAtom(first,rule,0);
        else
            pickAtom(first,rule,1);
    }
    //Atom type
    else
        InsertAntecedentOrConsequent(rule,first,isConsequent);

    //Right part
    if(_treeBuilder.getNodeAt(entry).getSecond() < 0)
        return;
    int second=_treeBuilder.getNodeAt(entry).getSecond();
    if(second != 1)
    {
        if(_treeBuilder.getNodeAt(second).getType() != ATOM)
        {
            if(!isConsequent)
                pickAtom(second,rule,0);
            else
                pickAtom(second,rule,1);
        }
        else
            InsertAntecedentOrConsequent(rule,second,isConsequent);
    }
}

void Workspace::resetConclusions(){
    _conclusions.clear();

    QList<int> consequentsAtoms=_mMConsequents.values();
    QList<int> antecedentsAtoms=_mMapAntecedents.values();

    for(unsigned int i=0U;i<consequentsAtoms.size();++i)
        if(!antecedentsAtoms.contains(consequentsAtoms.at(i)))
            if(!_conclusions.contains(consequentsAtoms.at(i)))
                _conclusions.append(consequentsAtoms.at(i));
}

void Workspace::resetDeniedAtoms(){
    for(qsizetype i=0; i<_treeBuilder.getTreeSize();++i)
        if(_treeBuilder.getNodeAt(i).getType() == NOT)
            _deniedAtoms.append(_treeBuilder.getNodeAt(i).getFirst());
}
