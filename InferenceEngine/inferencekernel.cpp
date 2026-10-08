#include <QTextStream>
#include <QDebug>
#include "inferencekernel.h"
#include <algorithm>

InferenceKernel::InferenceKernel(const PROPSyntaxAnalyzer *const pSyntaxAnalyzer) : _syntaxAnalyzer(pSyntaxAnalyzer), _workspace(new Workspace(pSyntaxAnalyzer->getTreeBuilder())), _isInferenceActive(true)
{
}

bool InferenceKernel::askForValue(int entry)
{
    if (entry < 0)
        return false;

    const QString symbol = _syntaxAnalyzer->getSymbolAt(_syntaxAnalyzer->getNodeAt(entry).getFirst());

    if (_workspace->_askedAtoms.contains(entry))
    {
        qDebug() << "These atoms are asked already" << symbol;
        return false;
    }

    QTextStream out(stdout);
    QTextStream in(stdin);

    out << "Asking for the value of " << symbol << '\n'
        << "Do you know the value?\n"
        << "1. Yes    2. No\n";

    QString selection;
    if (!(in >> selection))
        return false;

    if (selection != "1" && selection.compare("Yes", Qt::CaseInsensitive) != 0)
    {
        _workspace->_askedAtoms.append(entry);
        return false;
    }

    out << "Which is its value? (1 or 0)\n";

    QString valueSelection;
    if (!(in >> valueSelection))
        return false;

    if (valueSelection.toInt() == 1 || valueSelection.toInt() == 0)
    {
        setValue2Atom(entry, valueSelection.toInt());
        _workspace->_askedAtoms.append(entry);
        return true;
    }
    else
    {
        qDebug() << "Invalid value for atom:" << symbol << Qt::endl;
        return false;
    }
    return false;
}

bool InferenceKernel::setValue2Atom(int entry, bool value)
{
    if (entry < 0)
    {
        qDebug() << "The atom doesn't exist in the tree" << Qt::endl;
        return false;
    }    
    QString relAtom = _syntaxAnalyzer->getSymbolAt(_syntaxAnalyzer->getNodeAt(entry).getFirst());
    if (!_workspace->_mRelevantAtoms.contains(entry))
    {
        _workspace->_mRelevantAtoms.insert(entry, value);
        qDebug() << "Relevant atom added: " << relAtom << Qt::endl;
        return true;
    }
    if (_workspace->_mRelevantAtoms.value(entry) != value)
    {
        qDebug() << "Invalid attempt to set different value to the atom" << relAtom << Qt::endl;
        return false;
    }
    qDebug() << "Atom is in the container already. Ignored: " << relAtom << Qt::endl;
    return false;
}

void InferenceKernel::propagate(int entry, bool value)
{
    if (entry < 0)
        return;
    PROPNode Node = _syntaxAnalyzer->getNodeAt(entry);

    switch (Node.getType())
    {
    case AND:
    case OR:
    case IF:
    case IFF:
        propagate(Node.getFirst(), value);
        propagate(Node.getSecond(), value);
        break;
    case NOT:
        if (value == 1)
            value = 0;
        propagate(Node.getFirst(), value);
        break;
    case ATOM:
        _workspace->_mRelevantAtoms.insert(entry, value);
        break;
    case MAX_NODE_TYPE:
    case TRUE:
        break;
    }
}

void InferenceKernel::infer(int entry)
{
    const PROPNode Node = _syntaxAnalyzer->getNodeAt(entry);
    bool isMT = false;

    // Modus Tollens
    if (_workspace->_mConsequentsValues.contains(entry))
    {
        isMT = true;
        _workspace->_mAntecedentsValues.insert(entry, 0);
    }

    // Modus Ponens
    if (_workspace->_mAntecedentsValues.contains(entry))
        _workspace->_mConsequentsValues.insert(entry, 1);
    (isMT) ? propagate(Node.getFirst(), 0) : propagate(Node.getSecond(), 1);
}

int InferenceKernel::eval(int entry, Workspace::RuleType ruleType)
{
    const PROPNode Node = _syntaxAnalyzer->getNodeAt(entry);

    if (Node.getType() == ATOM)
    {
        if (_workspace->_mRelevantAtoms.contains(entry))
            return _workspace->_mRelevantAtoms.value(entry);
        else
            return -1;
    }

    int first = eval(Node.getFirst(), ruleType);
    int second;

    if (Node.getType() != NOT)
    {
        second = eval(Node.getSecond(), ruleType);
    }

    switch (Node.getType())
    {
    case IF:
        if (ruleType == Workspace::RuleType::ModusTollens)
        {
            qDebug() << "The consequent of rule\t" << _workspace->_rules.indexOf(entry) + 1 << "has a value of:\t" << second << Qt::endl;
            if (second == 1 || second == 0)
                _workspace->_mConsequentsValues.insert(entry, second);
            else
                return -1;
            return second;
        }
        // is Antecedent
        qDebug() << "The antecedent of rule\t" << _workspace->_rules.indexOf(entry) + 1 << "has a value of:\t" << first << Qt::endl;

        if (first != 1)
            return -1;
        else if (first == 0 &&
                 _workspace->_mRuleTypes.value(entry) == Workspace::RuleType::ModusPonens)
        {
            _workspace->_irrelevantRules.append(entry);
            // bug?
            _workspace->_mMapAntecedents.insert(entry, 0);
        }
        // bug as above, different container to same logic?
        _workspace->_mAntecedentsValues.insert(entry, 1);

        return first;
        break;
    case AND:
        if (first == 0 && second == 0)
            return 0;
        if (first == 1 && second == 1)
            return 1;
        return -1;
        break;

    case OR:
        if (first == 1 || second == 1)
            return 1;
        if (first == -1 || second == -1)
            return -1;
        return 0;
        break;

    case NOT:
        if (first == 1)
            return 0;
        if (first == 0)
            return 1;
        return -1;
        break;

    case IFF:
        if ((first != 1 && second != 1) || (first == 1 && second == 1))
            return 1;
        if (first == -1 || second == -1)
            return -1;
        return 0;
        break;

    case TRUE:
        return 1;
        break;

    default:
        qDebug() << "Unsopported Type or unknown error at eval()" << Qt::endl;
    }
    return 0;
}

// a1&a2&a3&a4
bool InferenceKernel::isCube(int entry)
{
    const PROPNode node = _syntaxAnalyzer->getNodeAt(entry);

    int first;
    int second;

    switch (node.getType())
    {
    case NOT:
        first = isCube(node.getFirst());

        if (first == 1)
            return 1;
        else
            return 0;
        break;

    case ATOM:
        return 1;
        break;

    case AND:
        first = isCube(node.getFirst());
        second = isCube(node.getSecond());

        if (first == 1 && second == 1)
            return 1;
        else
            return 0;
        break;

    case OR:
    case IF:
    case IFF:
    case TRUE:
    default:
        return 0;
        break;
    }
    return 0;
}

// a1||a2||a3||a4||a5
bool InferenceKernel::isClousure(int entry)
{
    const PROPNode node = _syntaxAnalyzer->getNodeAt(entry);

    int first;
    int second;

    switch (node.getType())
    {
    case NOT:
        first = isClousure(node.getFirst());
        if (first == 1)
            return 1;
        else
            return 0;
        break;

    case ATOM:
        return 1;
        break;

    case OR:
        first = isClousure(node.getFirst());
        second = isClousure(node.getSecond());

        if (first == 1 && second == 1)
            return 1;
        else
            return 0;
        break;

    case AND:
    case IF:
    case IFF:
    case TRUE:
    default:
        return 0;
        break;
    }
    return 0;
}

void promptError(int error)
{
    if (error == 0)
        qDebug() << "The entry is not correct for this function" << Qt::endl;
}

bool InferenceKernel::isAbleModusPonens(int entry)
{
    logOnlyForFunctions(Q_FUNC_INFO);

    const PROPNode Node = _syntaxAnalyzer->getNodeAt(entry);

    bool first;
    bool second;

    if (Node.getType() == IF)
    {
        first = isCube(Node.getFirst());
        second = isCube(Node.getSecond());

        if (first == false || second == false)
            return false;

        _workspace->_mRuleTypes.insert(entry, Workspace::RuleType::ModusPonens);
        qDebug() << "Rule: " << entry << " Is ModusPonens" << Qt::endl;
        return true;
    }
    else
    {
        promptError(0);
        return false;
    }
}

bool InferenceKernel::isAbleModusTollens(int entry)
{
    logOnlyForFunctions(Q_FUNC_INFO);

    const PROPNode Node = _syntaxAnalyzer->getNodeAt(entry);

    bool first;
    bool second;

    if (Node.getType() == IF)
    {
        first = isCube(Node.getFirst());
        second = isClousure(Node.getSecond());

        if (first == false || second == false)
            return false;

        _workspace->_mRuleTypes.insert(entry, Workspace::RuleType::ModusTollens);
        qDebug() << "Rule: " << entry << " Is ModusTollens" << Qt::endl;
        return true;
    }
    else
    {
        promptError(0);
        return false;
    }
}

void InferenceKernel::setAtomsToCheckFromConsequentsIfAbleMP(int entry)
{
    if (isAbleModusPonens(entry))
        _atomsToProcess = _workspace->_mMConsequents.values(entry);
}

void InferenceKernel::emitAtomValueDemandedFromNotRelevantandNotAskedAntecedentAtom(int currentAntAtom)
{
    if (!_workspace->_mRelevantAtoms.contains(currentAntAtom) &&
        !_workspace->_askedAtoms.contains(currentAntAtom))
    {
        _workspace->_askedAtoms.append(currentAntAtom);
        emit atomValueDemanded(currentAntAtom);
    }
}

void InferenceKernel::processAntecedentsValueToEmitAtomDemandedValue(int currentRuleEntry)
{
    for (unsigned int j = 0U; j < _workspace->_mMapAntecedents.values(currentRuleEntry).size(); ++j)
    {
        int currentAntAtom = _workspace->_mMapAntecedents.values(currentRuleEntry).at(j);
        emitAtomValueDemandedFromNotRelevantandNotAskedAntecedentAtom(currentAntAtom);
    }
}

QVector<int> InferenceKernel::getResettedRules() const
{
    _workspace->resetRules();
    return _workspace->_rules;
}

void InferenceKernel::setAtomsToCheckFromAntecedentsIfAbleMT(int entry)
{
    if (isAbleModusTollens(entry))
        _atomsToProcess = _workspace->_mMapAntecedents.values(entry);
}

void InferenceKernel::logOnlyForFunctions(const char *funcName)
{
    qDebug() << funcName << Qt::endl;
    ;
}

void InferenceKernel::AppendAtomEntriesToConclusionBasedInConclusionandRelevantAtomsContainment()
{
    logOnlyForFunctions(Q_FUNC_INFO);
    qDebug() << "Atoms entry to check in AppendAtomEntriesToConclusionBasedInConclusionandRelevantAtomsContainment"
             << _atomsToProcess.size()
             << "\n";
    qDebug() << "Conclusions contains:" << _workspace->_conclusions.size();
    qDebug() << "Revelant atoms contains:" << _workspace->_mRelevantAtoms.size();
    qDebug() << "Atoms to check contains:" << _atomsToProcess.size();

    foreach (int entryAtom, _atomsToProcess.toVector())
    {
        qDebug() << "entryAtom in COnclusion? " << _workspace->_conclusions.contains(entryAtom);
        qDebug() << "entryAtom in RelevantAtoms? " << _workspace->_mRelevantAtoms.contains(entryAtom);
        if (_workspace->_conclusions.contains(entryAtom) &&
            _workspace->_mRelevantAtoms.contains(entryAtom))
        {
            _conclusionsToProcess.append(entryAtom);
            qDebug() << "AppendAtomEntriesToConclusionBasedInCo"
                        "nclusionandRelevantAtomsContainment::a"
                        "pending in Conclusion \n";
        }
    }
}

void InferenceKernel::AppendToConclusionToDisplayAndConclusionValuesIfEntryIsNotInConclusionValues()
{
    logOnlyForFunctions(Q_FUNC_INFO);

    foreach (int entryConclusion, _conclusionsToProcess.toVector())
    {
        qDebug() << "AppendToConclusionToDisplayAndConclusionValuesIfEntryIsNotInConclusionValues loop \n";
        if (!_workspace->_mConclusionsValues.contains(entryConclusion))
        {
            qDebug() << "conclusionToDisplay will append a conclusion \n";
            _workspace->_mConclusionsValues.insert(entryConclusion,
                                                   _workspace->_mRelevantAtoms.value(entryConclusion));
            _conclusionsToDisplay.append(entryConclusion);
        }
    }
}

void InferenceKernel::EmitDisplayConclusionIfThereAreConclusionsToDisplay()
{
    logOnlyForFunctions(Q_FUNC_INFO);
    if (!_conclusionsToDisplay.isEmpty())
    {
        qDebug() << "About to emit displayConclusions signal \n";
        emit displayConclusions(_conclusionsToDisplay);
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

    if (isAbleModusPonens(entry))
        l_atomsToCheck = _workspace->_mMConsequents.values(entry);
    else if (isAbleModusTollens(entry))
        l_atomsToCheck = _workspace->_mMapAntecedents.values(entry);

    foreach (int entryAtom, l_atomsToCheck)
    {
        if (_workspace->_conclusions.contains(entryAtom) &&
            _workspace->_mRelevantAtoms.contains(entryAtom))
            l_conclusionToCheck.append(entryAtom);
    }

    foreach (int entryConclusion, l_conclusionToCheck)
    {
        if (!_workspace->_mConclusionsValues.contains(entryConclusion))
        {
            _workspace->_mConclusionsValues.insert(entryConclusion,
                                                   _workspace->_mRelevantAtoms.value(entryConclusion));
            l_displayConclusions.append(entryConclusion);
        }
    }

    if (!l_displayConclusions.isEmpty())
        emit displayConclusions(l_displayConclusions);

    return _isInferenceActive;
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
    _conclusionsToProcess.clear();
    _atomsToProcess.clear();
    _conclusionsToDisplay.clear();
}

void InferenceKernel::passToInferredRule(int currentRuleEntry)
{
    infer(currentRuleEntry);
    _workspace->_inferredRules.append(currentRuleEntry);
}

void InferenceKernel::determineIfAntecedentPresent(int currentRuleEntry)
{
    if (_workspace->_mAntecedentsValues.value(currentRuleEntry) == 1)
        passToInferredRule(currentRuleEntry);
}

void InferenceKernel::determineIfConsequentPresent(int currentRuleEntry)
{
    if (_workspace->_mConsequentsValues.value(currentRuleEntry) == 0 &&
        _workspace->_mConsequentsValues.contains(currentRuleEntry))
        passToInferredRule(currentRuleEntry);
}

void InferenceKernel::emitAtomValueDemandedFromNotRelevantNotAskedConsequentAtom(int currentConsqAtom)
{
    _workspace->_askedAtoms.append(currentConsqAtom);
    emit atomValueDemanded(currentConsqAtom);
}

void InferenceKernel::processConsequentsValueToEmitDemandedValue(int currentRuleEntry)
{
    for (unsigned int j = 0U; j < _workspace->_mMConsequents.values(currentRuleEntry).size(); ++j)
    {
        int currentConsqAtom = _workspace->_mMConsequents.values(currentRuleEntry).at(j);
        if (!_workspace->_mRelevantAtoms.contains(currentConsqAtom) &&
            !_workspace->_askedAtoms.contains(currentConsqAtom))
        {
            emitAtomValueDemandedFromNotRelevantNotAskedConsequentAtom(currentConsqAtom);
        }
    }
}

void InferenceKernel::inferConsequents(int currentRuleEntry)
{
    processConsequentsValueToEmitDemandedValue(currentRuleEntry);
    eval(currentRuleEntry, Workspace::RuleType::ModusTollens);
    determineIfConsequentPresent(currentRuleEntry);
}

void InferenceKernel::inferAntecedents(int currentRuleEntry)
{
    processAntecedentsValueToEmitAtomDemandedValue(currentRuleEntry);
    eval(currentRuleEntry, Workspace::RuleType::ModusPonens);
    determineIfAntecedentPresent(currentRuleEntry);
}

void InferenceKernel::inferCurrentRule(int conclusionFound, int currentRuleEntry)
{
    if (!_workspace->_inferredRules.contains(currentRuleEntry) &&
        !_workspace->_irrelevantRules.contains(currentRuleEntry))
    {
        // ModusPonens
        if (isAbleModusPonens(currentRuleEntry))
            inferAntecedents(currentRuleEntry);

        else if (isAbleModusTollens(currentRuleEntry))
            inferConsequents(currentRuleEntry);

        qDebug() << "The rule is not able to be calculated by MP or MT" << Qt::endl;
        conclusionFound = areVerifiedNewConclusions(currentRuleEntry);
    }
}

void InferenceKernel::iterateRules(const QVector<int> &rules, int conclusionFound)
{
    for (unsigned int i = 0U; i < rules.size(); ++i)
    {
        int currentRuleEntry = rules.at(i);

        inferCurrentRule(conclusionFound, currentRuleEntry);

        if (!conclusionFound)
        {
            qDebug() << "Conclusion was found in ForwardChainning" << Qt::endl;
            break;
        }
    }
}

void InferenceKernel::continueInferenceProcess()
{
    qDebug() << "Not Conclusion found already" << Qt::endl;
    _isInferenceActive = true;
}

void InferenceKernel::stopByKnowledgeExhausted()
{
    qDebug() << "There is no more rules to infer, The knowledge is exhausted" << Qt::endl;
    _isInferenceActive = false;
    emit knowledgeExhausted();
}

bool InferenceKernel::doesConclusionExists(int index)
{
    logOnlyForFunctions(Q_FUNC_INFO);
    foreach (int index, _workspace->_conclusions)
    {
        qDebug() << "doesCOnclusionsExists index test" << index;
    }

    return _workspace->_conclusions.toList().contains(index);
}

void InferenceKernel::forwardChainning(const QVector<int> &rules)
{

    int conclusionFound;
    conclusionFound = 1;

    while (1)
    { // While there are unviewed rules.

        QList<int> l_oldInferedRules = _workspace->_inferredRules.toList();

        for (int i = 0; i < rules.size(); i++)
        { // For each rule in the knowledge
            int currentRule = rules.at(i);

            // Ifthisruleisinfered, thenwedontneedcheckagain
            if (!_workspace->_inferredRules.contains(currentRule) &&
                !_workspace->_irrelevantRules.contains(currentRule))
            {

                // Modus Ponens
                if (isAbleModusPonens(currentRule))
                {
                    for (int j = 0; j < _workspace->_mMapAntecedents.values(currentRule).size(); j++)
                    {
                        int currentAntAtom = _workspace->_mMapAntecedents.values(currentRule).at(j);
                        if (!_workspace->_mRelevantAtoms.contains(currentAntAtom) &&
                            !_workspace->_askedAtoms.contains(currentAntAtom))
                        {
                            _workspace->_askedAtoms.append(currentAntAtom);
                            emit atomValueDemanded(currentAntAtom);
                        }
                    }

                    eval(currentRule, Workspace::RuleType::ModusPonens);
                    if (_workspace->_mAntecedentsValues.value(currentRule) == 1)
                    {
                        infer(currentRule);
                        _workspace->_inferredRules.append(currentRule);
                    }
                } // end Modus Tollens

                // Modus Tollens
                else if (isAbleModusTollens(currentRule))
                {
                    for (int j = 0; j < _workspace->_mMConsequents.values(currentRule).size(); j++)
                    {
                        int currentConsqAtom = _workspace->_mMConsequents.values(currentRule).at(j);
                        if (!_workspace->_mRelevantAtoms.contains(currentConsqAtom) &&
                            !_workspace->_askedAtoms.contains(currentConsqAtom))
                        {
                            _workspace->_askedAtoms.append(currentConsqAtom);
                            emit atomValueDemanded(currentConsqAtom);
                        }
                    }
                    eval(currentRule, Workspace::RuleType::ModusTollens); //
                    if (_workspace->_mConsequentsValues.value(currentRule) == 0 &&
                        _workspace->_mConsequentsValues.contains(currentRule))
                    {
                        infer(currentRule);
                        _workspace->_inferredRules.append(currentRule);
                    }
                }

                conclusionFound = areVerifiedNewConclusions(currentRule);

            } // end if infered rule

            if (!conclusionFound)
                break;
        } // end loop rules. Step 3

        // Si se ha seleccionado en una conclusion, entonces pausamos la inferencia.
        if (!conclusionFound)
        {
            qDebug() << "Not Conclusion Found  already" << Qt::endl;
            _isInferenceActive = true;
            return;
        }

        if (_workspace->_inferredRules.size() == l_oldInferedRules.size())
        {
            qDebug() << "There is no more rules to infer, The knowledge is exhausted" << Qt::endl;
            _isInferenceActive = false;
            emit knowledgeExhausted();
            return;
        }
    }
}

// void InferenceKernel::forwardChainning(const QVector<int> &rules)
//{
//     qDebug()<<"ForwardChainning entering \n";
//     logOnlyForFunctions(Q_FUNC_INFO);
//     int conclusionFound=1;

//    while(1){
//        int oldInferredRulesAmount=m_pWS->_inferredRules.size();

//        iterateRules(rules, conclusionFound);

//        if(!conclusionFound){
//            continueInferenceProcess();
//            return;
//        }

//        if(m_pWS->_inferredRules.size()== oldInferredRulesAmount){
//            stopByKnowledgeExhausted();
//            return;
//        }
//    }
//}

void InferenceKernel::backwardChainning(QSet<int> k)
{
    qDebug() << "Entering to Backward Chainning \n";
    QSet<int> CRDI;
    CRDI.clear();
    QSet<int> oldCRDI;

    do
    {
        oldCRDI = CRDI;
        for (unsigned int i = 0; i < _workspace->_rules.size(); ++i)
        {
            int currentRule = _workspace->_rules.at(i);

            QList<int> values = _workspace->_mMConsequents.values(currentRule);
            QSet<int> tempCqs(values.begin(), values.end());

            QList<int> values2 = _workspace->_mMapAntecedents.values(currentRule);
            QSet<int> tempAnt(values2.begin(), values2.end());

            if (tempCqs.intersect(k).size() != 0 &&
                isAbleModusPonens(currentRule))
            {
                k.unite(tempAnt);
                CRDI.insert(currentRule);
            }
            else if (tempAnt.intersect(k).size() != 0 && isAbleModusTollens(currentRule))
            {
                k.unite(tempCqs);
                CRDI.insert(currentRule);
            }
        }
    } while (CRDI.size() != oldCRDI.size());

    QList<int> resBW(CRDI.begin(), CRDI.end());
    std::sort(resBW.begin(), resBW.end());

    forwardChainning(resBW.toVector());
}

InferenceKernel::~InferenceKernel()
{
    delete _workspace;
}
