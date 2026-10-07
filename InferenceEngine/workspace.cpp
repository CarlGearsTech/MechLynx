#include "workspace.h"
#include "proptreebuilder.h"


void Workspace::InsertAntecedentOrConsequent(int rule, int entry, bool isConsequent){
    if(!isConsequent)
        m_mmAntecedents.insert(rule,entry);
    else
        m_mmConsequents.insert(rule,entry);
}

void Workspace::reset(){
    resetRules();
    resetAntecedentsAndConsequents();
    resetConclusions();
    resetDeniedAtoms();
    resetRules();
    m_mRuleTypes.clear();
    m_mAntecedentsValue.clear();
    m_mConsequentsValue.clear();
    m_vAskedAtoms.clear();
    m_vInferredRules.clear();
    m_vIrrelevantRules.clear();
    m_mConclusionValues.clear();
    m_mRelevantAtoms.clear();
}

void Workspace::resetRules()
{
    m_vRules.clear();
    for(qsizetype i=0; i < _treeBuilder.getTreeSize() ; ++i)
    {
        const auto& node = _treeBuilder.getNodeAt(i);
        if(node.getType() == IF)
            m_vRules.append(i);
    }
}

void Workspace::resetAntecedentsAndConsequents(){
    m_mmAntecedents.clear();
    m_mConsequentsValue.clear();

    for(int i=0, lentry=0,rentry=0;i<m_vRules.size();++i){
        lentry=_treeBuilder.getNodeAt(m_vRules.at(i)).getFirst();
        pickAtom(lentry,m_vRules.at(i),0);

        rentry=_treeBuilder.getNodeAt(m_vRules.at(i)).getSecond();
        pickAtom(rentry,m_vRules.at(i),1);
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
    m_vConclusions.clear();

    QList<int> consequentsAtoms=m_mmConsequents.values();
    QList<int> antecedentsAtoms=m_mmAntecedents.values();

    for(unsigned int i=0U;i<consequentsAtoms.size();++i)
        if(!antecedentsAtoms.contains(consequentsAtoms.at(i)))
            if(!m_vConclusions.contains(consequentsAtoms.at(i)))
                m_vConclusions.append(consequentsAtoms.at(i));
}

void Workspace::resetDeniedAtoms(){
    for(qsizetype i=0; i<_treeBuilder.getTreeSize();++i)
        if(_treeBuilder.getNodeAt(i).getType() == NOT)
            m_vDeniedAtoms.append(_treeBuilder.getNodeAt(i).getFirst());
}
