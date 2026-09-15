#include <QTextStream>
#include <QDebug>
#include "inferencekernel.h"
#include <algorithm>

//Remember that you change the Worksapce constructor to private,
//Making IK friend of Workspace and having interface functions to Workspace
//from IK
InferenceKernel::InferenceKernel(PROPTreeBuilder *tb):
    m_pTB(tb),m_pWS(new Workspace(m_pTB)),m_bContinueInferenceProcess(true)
{
}

bool InferenceKernel::askForValue(int entry){
    QTextStream out(stdout);
    QTextStream in(stdin);
    QString selection;
    QString symbol= m_pTB->getSymbols().at(m_pTB->getTree().at(entry).first());

    if(m_pWS->m_vAskedAtoms.contains(entry)){
        qDebug()<<"These atoms are asked already"<<symbol<<Qt::endl;
        return false;
    }

    m_pWS->m_vAskedAtoms.append(entry);

    out<<"Asking for the value of"<<symbol<<Qt::endl;
    out<<"Do you know the value? \n"<<"1. Yes \t 2. No"<<Qt::endl;
     in>>selection;

    if(selection.toInt()==1 || selection=="Yes"){
        QString valueSelection;
        valueSelection.clear();

        out<<"Which is its value? \n 1 or 0"<<Qt::endl;
        in>>valueSelection;
        if(valueSelection.toInt()==1 || valueSelection.toInt()==0){
            setValue2Atom(entry,valueSelection.toInt());
            return true;
        }
        else{
            qDebug()<<"Invalid value for atom:"<<symbol<<Qt::endl;
            return false;
        }
    }
    return false;
}

bool InferenceKernel::setValue2Atom(int entry, bool value){
    if(entry == -1){
        qDebug()<<"The atom doesn't exist in the tree"<<Qt::endl;
        return false;
    }
    QString relAtom= m_pTB->getSymbols().at(m_pTB->getTree().at(entry).first());

    if(!m_pWS->m_mRelevantAtoms.contains(entry)){
        m_pWS->m_mRelevantAtoms.insert(entry,value);
        qDebug()<<"Relevant atom added: "<<relAtom<<Qt::endl;
        return true;
    }
    if(m_pWS->m_mRelevantAtoms.value(entry) != value){
        qDebug()<<"Invalid attempt to set different value to the atom"<<
                  relAtom<<Qt::endl;
        return false;
    }
    qDebug()<<"Atom is in the container already. Ignored: "<<relAtom<<Qt::endl;
    return false;
}

void InferenceKernel::propagate(int entry, bool value){
    PROPNode Node=m_pTB->getTree().at(entry);

    switch(Node.getType()){
    case PROPNode::AND:
    case PROPNode::OR:
    case PROPNode::IF:
    case PROPNode::IFF:
        propagate(Node.first(),value);
        propagate(Node.second(),value);
        break;

    case PROPNode::NOT:
        if(value==1)
            value=0;
        propagate(Node.first(),value);
        break;

    case PROPNode::ATOM:
        m_pWS->m_mRelevantAtoms.insert(entry,value);
        break;
    case PROPNode::TRUE:
        break;
    }
}

void InferenceKernel::infer(int entry){
    PROPNode Node=m_pTB->getTree().at(entry);
    bool isMT=false;

    //Modus Tollens
    if(m_pWS->m_mConsequentsValue.contains(entry)){
        isMT=true;
        m_pWS->m_mAntecedentsValue.insert(entry,0);
    }

    //Modus Ponens
    if(m_pWS->m_mAntecedentsValue.contains(entry))
        m_pWS->m_mConsequentsValue.insert(entry,1);

    (isMT)?propagate(Node.first(),0):propagate(Node.second(),1);
}

int InferenceKernel::eval(int entry, Workspace::RuleType ruleType){
    PROPNode Node=m_pTB->getTree().at(entry);


    if(Node.getType()== PROPNode::ATOM){
        if(m_pWS->m_mRelevantAtoms.contains(entry))
            return m_pWS->m_mRelevantAtoms.value(entry);
        else
            return -1;
    }

    int first=eval(Node.first(),ruleType);
    int second;

    if(Node.getType()!=PROPNode::NOT){
        second=eval(Node.second(),ruleType);
    }

    switch(Node.getType()){
    case PROPNode::IF:
        if(ruleType == Workspace::RuleType::ModusTollens){
            qDebug()<<"The consequent of rule\t"<< m_pWS->m_vRules.indexOf(entry)+1<<
                      "has a value of:\t"<<second<<Qt::endl;
            if(second==1 || second==0)
                m_pWS->m_mConsequentsValue.insert(entry,second);
            else
                return -1;
            return second;
        }
        //is Antecedent
        qDebug()<<"The antecedent of rule\t"<< m_pWS->m_vRules.indexOf(entry)+1<<
                  "has a value of:\t"<<first<<Qt::endl;

        if(first != 1)
            return -1;
        else if(first==0 &&
                m_pWS->m_mRuleTypes.value(entry)==Workspace::RuleType::ModusPonens){
            m_pWS->m_vIrrelevantRules.append(entry);
            //bug?
            m_pWS->m_mmAntecedents.insert(entry,0);
        }
        //bug as above, different container to same logic?
        m_pWS->m_mAntecedentsValue.insert(entry,1);

        return first;
        break;
    case PROPNode::AND:
        if(first==0 && second==0)
            return 0;
        if(first==1 && second== 1)
            return 1;
        return -1;
        break;

    case PROPNode::OR:
        if(first ==1 || second==1)
            return 1;
        if(first==-1 || second==-1)
            return -1;
        return 0;
        break;

    case PROPNode::NOT:
        if(first==1)
            return 0;
        if(first == 0)
            return 1;
        return -1;
        break;

    case PROPNode::IFF:
        if((first!= 1 && second != 1)||(first==1 && second==1))
            return 1;
        if(first==-1 || second==-1)
            return -1;
        return 0;
        break;

    case PROPNode::TRUE:
        return 1;
        break;

    default:
        qDebug()<<"Unsopported Type or unknown error at eval()"<<Qt::endl;
    }
    return 0;
}

//a1&a2&a3&a4
bool InferenceKernel::isCube(int entry){
    PROPNode node=m_pTB->getTree().at(entry);

    int first;
    int second;

    switch(node.getType()){
    case PROPNode::NOT:
        first=isCube(node.first());

        if(first==1)
            return 1;
        else
            return 0;
        break;

    case PROPNode::ATOM:
        return 1;
        break;

    case PROPNode::AND:
        first=isCube(node.first());
        second=isCube(node.second());

        if(first==1 && second==1)
            return 1;
        else
            return 0;
        break;

    case PROPNode::OR:
    case PROPNode::IF:
    case PROPNode::IFF:
    case PROPNode::TRUE:
    default:
        return 0;
        break;
    }
    return 0;
}

//a1||a2||a3||a4||a5
bool InferenceKernel::isClousure(int entry){
    PROPNode node=m_pTB->getTree().at(entry);

    int first;
    int second;

    switch(node.getType()){
    case PROPNode::NOT:
        first=isClousure(node.first());
        if(first==1)
            return 1;
        else
            return 0;
        break;

    case PROPNode::ATOM:
        return 1;
        break;

    case PROPNode::OR:
        first=isClousure(node.first());
        second=isClousure(node.second());

        if(first==1 && second==1)
            return 1;
        else
            return 0;
        break;

    case PROPNode::AND:
    case PROPNode::IF:
    case PROPNode::IFF:
    case PROPNode::TRUE:
    default:
        return 0;
        break;
    }
    return 0;
}

void promptError(int error){
    if(error == 0)
        qDebug()<<"The entry is not correct for this function"<<Qt::endl;
}

bool InferenceKernel::isAbleModusPonens(int entry){
    logOnlyForFunctions(Q_FUNC_INFO);

    PROPNode Node=m_pTB->getTree().at(entry);

    bool first;
    bool second;

    if(Node.getType()==PROPNode::IF){
        first=isCube(Node.first());
        second=isCube(Node.second());

        if(first==false || second==false)
            return false;

        m_pWS->m_mRuleTypes.insert(entry,Workspace::RuleType::ModusPonens);
        qDebug()<<"Rule: "<<entry<<" Is ModusPonens"<<Qt::endl;
        return true;
    }
    else{
        promptError(0);
        return false;
    }
}

bool InferenceKernel::isAbleModusTollens(int entry){
    logOnlyForFunctions(Q_FUNC_INFO);

    PROPNode Node=m_pTB->getTree().at(entry);

    bool first;
    bool second;

    if(Node.getType()==PROPNode::IF){
        first=isCube(Node.first());
        second=isClousure(Node.second());

        if(first==false || second==false)
            return false;

        m_pWS->m_mRuleTypes.insert(entry,Workspace::RuleType::ModusTollens);
        qDebug()<<"Rule: "<<entry<<" Is ModusTollens"<<Qt::endl;
        return true;
    }
    else{
        promptError(0);
        return false;
    }
}

void InferenceKernel::setAtomsToCheckFromConsequentsIfAbleMP(int entry)
{
    if(isAbleModusPonens(entry))
        atomsToCheck=m_pWS->m_mmConsequents.values(entry);
}

void InferenceKernel::emitAtomValueDemandedFromNotRelevantandNotAskedAntecedentAtom(int currentAntAtom)
{
    if(!m_pWS->m_mRelevantAtoms.contains(currentAntAtom) &&
            !m_pWS->m_vAskedAtoms.contains(currentAntAtom)){
        m_pWS->m_vAskedAtoms.append(currentAntAtom);
        emit atomValueDemanded(currentAntAtom);
    }
}

void InferenceKernel::processAntecedentsValueToEmitAtomDemandedValue(int currentRuleEntry)
{
    for(unsigned int j=0U; j<m_pWS->m_mmAntecedents.values(currentRuleEntry).size();++j){
        int currentAntAtom=m_pWS->m_mmAntecedents.values(currentRuleEntry).at(j);
        emitAtomValueDemandedFromNotRelevantandNotAskedAntecedentAtom(currentAntAtom);
    }
}


QVector<int> InferenceKernel::getResettedRules() const
{
    m_pWS->resetRules();
    return m_pWS->m_vRules;
}

void InferenceKernel::setAtomsToCheckFromAntecedentsIfAbleMT(int entry)
{
    if(isAbleModusTollens(entry))
        atomsToCheck=m_pWS->m_mmAntecedents.values(entry);
}


void InferenceKernel::logOnlyForFunctions(const char* funcName)
{
    qDebug()<<funcName<<Qt::endl;;
}

void InferenceKernel::AppendAtomEntriesToConclusionBasedInConclusionandRelevantAtomsContainment()
{
    logOnlyForFunctions(Q_FUNC_INFO);
    qDebug()<<"Atoms entry to check in AppendAtomEntriesToConclusionBasedInConclusionandRelevantAtomsContainment"
           <<atomsToCheck.size()
          <<"\n";
    qDebug()<<"Conclusions contains:"<<m_pWS->m_vConclusions.size();
    qDebug()<<"Revelant atoms contains:"<<m_pWS->m_mRelevantAtoms.size();
    qDebug()<<"Atoms to check contains:"<<atomsToCheck.size();

    foreach(int entryAtom,atomsToCheck.toVector()){
        qDebug()<<"entryAtom in COnclusion? "<<m_pWS->m_vConclusions.contains(entryAtom);
        qDebug()<<"entryAtom in RelevantAtoms? "<<m_pWS->m_mRelevantAtoms.contains(entryAtom);
        if(m_pWS->m_vConclusions.contains(entryAtom) &&
                m_pWS->m_mRelevantAtoms.contains(entryAtom)){
            conclusionsToCheck.append(entryAtom);
            qDebug()<<"AppendAtomEntriesToConclusionBasedInCo"
                      "nclusionandRelevantAtomsContainment::a"
                      "pending in Conclusion \n";
        }
    }
}

void InferenceKernel::AppendToConclusionToDisplayAndConclusionValuesIfEntryIsNotInConclusionValues()
{
    logOnlyForFunctions(Q_FUNC_INFO);

    foreach(int entryConclusion, conclusionsToCheck.toVector()){
        qDebug()<<"AppendToConclusionToDisplayAndConclusionValuesIfEntryIsNotInConclusionValues loop \n";
        if(!m_pWS->m_mConclusionValues.contains(entryConclusion)){
            qDebug()<<"conclusionToDisplay will append a conclusion \n";
            m_pWS->m_mConclusionValues.insert(entryConclusion,
                                              m_pWS->m_mRelevantAtoms.value(entryConclusion));
            conclusionsToDisplay.append(entryConclusion);
        }
    }
}

void InferenceKernel::EmitDisplayConclusionIfThereAreConclusionsToDisplay()
{
    logOnlyForFunctions(Q_FUNC_INFO);
    if(!conclusionsToDisplay.isEmpty()){
        qDebug()<<"About to emit displayConclusions signal \n";
        emit displayConclusions(conclusionsToDisplay);
    }
}


bool InferenceKernel::areVerifiedNewConclusions(int entry)
{
    QList<int> l_conclusionToCheck;
    QList<int> l_atomsToCheck;
    QList<int> l_displayConclusions;
    l_atomsToCheck.clear();
    l_conclusionToCheck.clear();
    l_displayConclusions.clear();

    if(isAbleModusPonens(entry))
        l_atomsToCheck=m_pWS->m_mmConsequents.values(entry);
    else if(isAbleModusTollens(entry))
        l_atomsToCheck=m_pWS->m_mmAntecedents.values(entry);

    foreach(int entryAtom,l_atomsToCheck){
        if(m_pWS->m_vConclusions.contains(entryAtom) &&
                m_pWS->m_mRelevantAtoms.contains(entryAtom))
            l_conclusionToCheck.append(entryAtom);
    }

    foreach (int entryConclusion, l_conclusionToCheck) {
        if(!m_pWS->m_mConclusionValues.contains(entryConclusion)){
            m_pWS->m_mConclusionValues.insert(entryConclusion,
                                                m_pWS->m_mRelevantAtoms.value(entryConclusion));
            l_displayConclusions.append(entryConclusion);
        }
    }

    if(!l_displayConclusions.isEmpty())
        emit displayConclusions(l_displayConclusions);

    return m_bContinueInferenceProcess;
//    logOnlyForFunctions(Q_FUNC_INFO);

//    prepareConclusionToBeDisplayed(entry);

//    EmitDisplayConclusionIfThereAreConclusionsToDisplay(); //Signal to declare

//    return m_bContinueInferenceProcess;
}

void InferenceKernel::prepareConclusionToBeDisplayed(int entry)
{
    clearConclusionsToCheckDisplayAndAtomsToCheck();

    setAtomsToCheckFromConsequentsIfAbleMP(entry);

    setAtomsToCheckFromAntecedentsIfAbleMT(entry);

    AppendAtomEntriesToConclusionBasedInConclusionandRelevantAtomsContainment();

    AppendToConclusionToDisplayAndConclusionValuesIfEntryIsNotInConclusionValues();
}

void InferenceKernel::clearConclusionsToCheckDisplayAndAtomsToCheck()
{
    conclusionsToCheck.clear();
    atomsToCheck.clear();
    conclusionsToDisplay.clear();
}


void InferenceKernel::passToInferredRule(int currentRuleEntry)
{
    infer(currentRuleEntry);
    m_pWS->m_vInferredRules.append(currentRuleEntry);
}

void InferenceKernel::determineIfAntecedentPresent(int currentRuleEntry)
{
    if(m_pWS->m_mAntecedentsValue.value(currentRuleEntry)==1)
        passToInferredRule(currentRuleEntry);

}

void InferenceKernel::determineIfConsequentPresent(int currentRuleEntry)
{
    if(m_pWS->m_mConsequentsValue.value(currentRuleEntry)==0 &&
            m_pWS->m_mConsequentsValue.contains(currentRuleEntry))
        passToInferredRule(currentRuleEntry);

}

void InferenceKernel::emitAtomValueDemandedFromNotRelevantNotAskedConsequentAtom(int currentConsqAtom)
{
    m_pWS->m_vAskedAtoms.append(currentConsqAtom);
    emit atomValueDemanded(currentConsqAtom);
}

void InferenceKernel::processConsequentsValueToEmitDemandedValue(int currentRuleEntry)
{
    for(unsigned int j=0U;j <m_pWS->m_mmConsequents.values(currentRuleEntry).size();++j){
        int currentConsqAtom=m_pWS->m_mmConsequents.values(currentRuleEntry).at(j);
        if(!m_pWS->m_mRelevantAtoms.contains(currentConsqAtom) &&
                !m_pWS->m_vAskedAtoms.contains(currentConsqAtom)){
            emitAtomValueDemandedFromNotRelevantNotAskedConsequentAtom(currentConsqAtom);
        }
    }
}

void InferenceKernel::inferConsequents(int currentRuleEntry)
{
    processConsequentsValueToEmitDemandedValue(currentRuleEntry);
    eval(currentRuleEntry,Workspace::RuleType::ModusTollens);
    determineIfConsequentPresent(currentRuleEntry);
}

void InferenceKernel::inferAntecedents(int currentRuleEntry)
{
    processAntecedentsValueToEmitAtomDemandedValue(currentRuleEntry);
    eval(currentRuleEntry,Workspace::RuleType::ModusPonens);
    determineIfAntecedentPresent(currentRuleEntry);
}

void InferenceKernel::inferCurrentRule(int conclusionFound, int currentRuleEntry)
{
    if(!m_pWS->m_vInferredRules.contains(currentRuleEntry) &&
            !m_pWS->m_vIrrelevantRules.contains(currentRuleEntry)){
        //ModusPonens
        if(isAbleModusPonens(currentRuleEntry))
            inferAntecedents(currentRuleEntry);

        else if(isAbleModusTollens(currentRuleEntry))
            inferConsequents(currentRuleEntry);

        qDebug()<<"The rule is not able to be calculated by MP or MT"<<Qt::endl;
        conclusionFound=areVerifiedNewConclusions(currentRuleEntry);
    }
}


void InferenceKernel::iterateRules(const QVector<int> &rules, int conclusionFound)
{
    for(unsigned int i=0U; i< rules.size();++i){
        int currentRuleEntry=rules.at(i);

        inferCurrentRule(conclusionFound, currentRuleEntry);

        if(!conclusionFound){
            qDebug()<<"Conclusion was found in ForwardChainning"<<Qt::endl;
            break;
        }
    }
}



void InferenceKernel::continueInferenceProcess()
{
    qDebug()<<"Not Conclusion found already"<<Qt::endl;
    m_bContinueInferenceProcess=true;
}

void InferenceKernel::stopByKnowledgeExhausted()
{
    qDebug()<<"There is no more rules to infer, The knowledge is exhausted"<<Qt::endl;
    m_bContinueInferenceProcess=false;
    emit knowledgeExhausted();
}

bool InferenceKernel::doesConclusionExists(int index){
    logOnlyForFunctions(Q_FUNC_INFO);
    foreach(int index,m_pWS->m_vConclusions){
        qDebug()<<"doesCOnclusionsExists index test"<<index;
    }

    return m_pWS->m_vConclusions.toList().contains(index);
}

void InferenceKernel::forwardChainning(const QVector<int> &rules){

    int conclusionFound;
    conclusionFound=1;

    while(1){ //While there are unviewed rules.

        QList<int> l_oldInferedRules=m_pWS->m_vInferredRules.toList();

        for(int i=0;i< rules.size();i++){ //For each rule in the knowledge
            int currentRule=rules.at(i);

            //Ifthisruleisinfered, thenwedontneedcheckagain
            if(!m_pWS->m_vInferredRules.contains(currentRule) &&
                    !m_pWS->m_vIrrelevantRules.contains(currentRule)){

                //Modus Ponens
                if(isAbleModusPonens(currentRule)){
                    for(int j=0;j<m_pWS->m_mmAntecedents.values(currentRule).size();j++){
                           int currentAntAtom=m_pWS->m_mmAntecedents.values(currentRule).at(j);
                        if(!m_pWS->m_mRelevantAtoms.contains(currentAntAtom) &&
                                !m_pWS->m_vAskedAtoms.contains(currentAntAtom)){
                            m_pWS->m_vAskedAtoms.append(currentAntAtom);
                            emit atomValueDemanded(currentAntAtom);
                        }
                    }

                   eval(currentRule,Workspace::RuleType::ModusPonens);
                    if(m_pWS->m_mAntecedentsValue.value(currentRule)==1){
                        infer(currentRule);
                        m_pWS->m_vInferredRules.append(currentRule);
                    }
                }//end Modus Tollens

                //Modus Tollens
                else if(isAbleModusTollens(currentRule)){
                    for(int j=0;j<m_pWS->m_mmConsequents.values(currentRule).size();j++){
                        int currentConsqAtom=m_pWS->m_mmConsequents.values(currentRule).at(j);
                        if(!m_pWS->m_mRelevantAtoms.contains(currentConsqAtom) &&
                                !m_pWS->m_vAskedAtoms.contains(currentConsqAtom)){
                            m_pWS->m_vAskedAtoms.append(currentConsqAtom);
                            emit atomValueDemanded(currentConsqAtom);
                        }
                    }
                    eval(currentRule,Workspace::RuleType::ModusTollens);//
                    if(m_pWS->m_mConsequentsValue.value(currentRule)==0 &&
                            m_pWS->m_mConsequentsValue.contains(currentRule)){
                        infer(currentRule);
                        m_pWS->m_vInferredRules.append(currentRule);
                    }
                }

                conclusionFound=areVerifiedNewConclusions(currentRule);

          }//end if infered rule

            if(!conclusionFound)
                break;
        }//end loop rules. Step 3

        //Si se ha seleccionado en una conclusion, entonces pausamos la inferencia.
        if(!conclusionFound){
            qDebug()<<"Not Conclusion Found  already"<<Qt::endl;
            m_bContinueInferenceProcess=true;
            return;
        }

        if(m_pWS->m_vInferredRules.size()==l_oldInferedRules.size() ){
         qDebug()<<"There is no more rules to infer, The knowledge is exhausted"<<Qt::endl;
         m_bContinueInferenceProcess=false;
         emit knowledgeExhausted();
         return;
        }
    }
}


//void InferenceKernel::forwardChainning(const QVector<int> &rules)
//{
//    qDebug()<<"ForwardChainning entering \n";
//    logOnlyForFunctions(Q_FUNC_INFO);
//    int conclusionFound=1;

//    while(1){
//        int oldInferredRulesAmount=m_pWS->m_vInferredRules.size();

//        iterateRules(rules, conclusionFound);

//        if(!conclusionFound){
//            continueInferenceProcess();
//            return;
//        }

//        if(m_pWS->m_vInferredRules.size()== oldInferredRulesAmount){
//            stopByKnowledgeExhausted();
//            return;
//        }
//    }
//}

void InferenceKernel::backwardChainning(QSet<int> k)
{
    qDebug()<<"Entering to Backward Chainning \n";
    QSet<int> CRDI;
    CRDI.clear();
    QSet<int> oldCRDI;

    do{
        oldCRDI=CRDI;
        for(unsigned int i=0; i<m_pWS->m_vRules.size();++i){
            int currentRule=m_pWS->m_vRules.at(i);

            QList<int> values = m_pWS->m_mmConsequents.values(currentRule);
            QSet<int> tempCqs(values.begin(), values.end());

            QList<int> values2 = m_pWS->m_mmAntecedents.values(currentRule);
            QSet<int> tempAnt(values2.begin(), values2.end());

            if(tempCqs.intersect(k).size() != 0 &&
                    isAbleModusPonens(currentRule)){
                k.unite(tempAnt);
                CRDI.insert(currentRule);
            }
            else if(tempAnt.intersect(k).size() != 0
                    && isAbleModusTollens(currentRule)){
                k.unite(tempCqs);
                CRDI.insert(currentRule);
            }
        }
    }while(CRDI.size() != oldCRDI.size());

    QList<int> resBW(CRDI.begin(), CRDI.end());
    std::sort(resBW.begin(), resBW.end());

    forwardChainning(resBW.toVector());
}

InferenceKernel::~InferenceKernel(){
    delete m_pWS;
}
