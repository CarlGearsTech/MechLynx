#include "infixsyntaxanalyzer.h"
#include <QDebug>

void InfixSyntaxAnalyzer::buildLeftToken(int &left, int &right, PROPLexAnalyzer::LEXEM &operation,
                                         unsigned int &signal,
                                         QStack<int> &builderStack,
                                         QStack<PROPLexAnalyzer::LEXEM>& tokenStack, bool &isValidated)const{

    if(operation.m_sToken=="!"){
        right=builderStack.pop();
        operation.m_ntreeIndex=m_pTB->Not(right);
    }
    else{
        if(!tokenStack.size()){
            signal=6;
            return;
        }
        if(!builderStack.size()){
            signal=7;
            return;
        }
        PROPLexAnalyzer::LEXEM leftToken=tokenStack.pop();

        switch (leftToken.m_type) {
        case PROPLexAnalyzer::LEXEM::ID:
            builderStack.push(m_pTB->atom(leftToken.m_sToken));
            left=builderStack.pop();
            right=builderStack.pop();
            isValidated=true;
            return;
        case PROPLexAnalyzer::LEXEM::OPERATOR:
            right=builderStack.pop();
            left=builderStack.pop();
            isValidated=true;
            break;
        case PROPLexAnalyzer::LEXEM::EOL:
        case PROPLexAnalyzer::LEXEM::OPENBRACKET:
        case PROPLexAnalyzer::LEXEM::CLOSEDBRAKET:
        case PROPLexAnalyzer::LEXEM::COMMENT:
        case PROPLexAnalyzer::LEXEM::ERROR:
        case PROPLexAnalyzer::LEXEM::ENDOFF:
        default:
            signal=2;
            break;
        }//end left token switch
    }
}

bool InfixSyntaxAnalyzer::Compile()const{
    unsigned int l_signal=0;

    QStack<PROPLexAnalyzer::LEXEM> l_tokenStack;
    l_tokenStack.clear();
    unsigned int l_nErrorRow=0;
    unsigned int l_nErrorCol=0;
    bool l_isRuleAvailable=false;
    bool l_isFirstRule=true;
    QStack<int> l_builderStack;
    l_builderStack.clear();
    unsigned int l_nClosedB=0;
    unsigned int l_nOpenB=0;
    bool l_isValidated=false;

    while(!l_signal){

        while(!l_signal){
            PROPLexAnalyzer::LEXEM L=m_pLA->getToken();
            if(L.m_type==PROPLexAnalyzer::LEXEM::EOL){
                ++l_nErrorRow;
                l_nErrorCol=0;
            }
            else if(L.m_type==PROPLexAnalyzer::LEXEM::ENDOFF){
                l_signal=4;
                break;
            }
            else{
                ++l_nErrorCol;

                if(L.m_sToken==";"){
                    if(l_isFirstRule){
                        l_isFirstRule=false;
                        l_builderStack.push(m_pTB->True());
                        l_tokenStack.push(m_pLA->buildToken(PROPLexAnalyzer::LEXEM::OPERATOR, "True"));
                    }
                    l_isRuleAvailable=true;
                    break;
                }
                else if(L.m_type==PROPLexAnalyzer::LEXEM::COMMENT){
                    ++l_nErrorRow;
                    continue;
                }
                else if(L.m_type==PROPLexAnalyzer::LEXEM::ERROR){
                    l_signal=2;
                    break;
                }
                else if(L.m_sToken==")"){
                    ++l_nClosedB;
                    break;
                }
                else if(L.m_sToken=="("){
                    ++l_nOpenB;
                    continue;
                }
                l_tokenStack.push(L);
            }//end toke else
        }//Rule while

        int l_right=0;
        int l_left=0;

        //EOF signal
        if(l_signal==4)
            break;

        //Too many semicolons
        if(l_isRuleAvailable && l_builderStack.size()<2){
            l_signal=11;
            break;
        }

        //No enough tokens to fit the operations
        if(l_signal==4 && l_tokenStack.size()<2 && l_builderStack.size() != 1){
            l_signal=5;
            break;
        }

        //Ensure operation with 2 operands
        if(l_tokenStack.size()<2 && l_builderStack.size() !=1){
            l_signal=8;
            break;
        }

        if(l_signal==2)
            break;

        PROPLexAnalyzer::LEXEM l_rightToken=l_tokenStack.pop();
        PROPLexAnalyzer::LEXEM l_operation= l_tokenStack.pop();

        if(l_isRuleAvailable){
            l_right=l_builderStack.pop();
            l_left=l_builderStack.pop();
            l_operation=m_pLA->buildToken(PROPLexAnalyzer::LEXEM::OPERATOR,"&");
            l_operation.m_ntreeIndex=m_pTB->And(l_left,l_right);
            l_isRuleAvailable=false;
        }
        else{
            switch (l_rightToken.m_type) {
            case PROPLexAnalyzer::LEXEM::ID:
                l_builderStack.push(m_pTB->atom(l_rightToken.m_sToken));
                buildLeftToken(l_left,l_right,l_operation,l_signal,l_builderStack,
                               l_tokenStack,l_isValidated);
                break;
            case PROPLexAnalyzer::LEXEM::OPERATOR:
                buildLeftToken(l_left,l_right,l_operation,l_signal,l_builderStack,
                               l_tokenStack,l_isValidated);
                break;
            case PROPLexAnalyzer::LEXEM::EOL:
            case PROPLexAnalyzer::LEXEM::OPENBRACKET:
            case PROPLexAnalyzer::LEXEM::CLOSEDBRAKET:
            case PROPLexAnalyzer::LEXEM::ERROR:
            case PROPLexAnalyzer::LEXEM::COMMENT:
            case PROPLexAnalyzer::LEXEM::ENDOFF:
            default:
                l_signal=2;
                break;
            }//end right token switch
            if(l_isValidated){
                if(l_operation.m_sToken=="->")
                    l_operation.m_ntreeIndex=m_pTB->If(l_left,l_right);
                else if(l_operation.m_sToken=="&")
                    l_operation.m_ntreeIndex=m_pTB->And(l_left,l_right);
                else if(l_operation.m_sToken=="|")
                    l_operation.m_ntreeIndex=m_pTB->Or(l_left,l_right);
                else if(l_operation.m_sToken=="<->")
                    l_operation.m_ntreeIndex=m_pTB->Iff(l_left,l_right);
                else
                    l_signal=2;
                l_isValidated=false;
            }
        }//end else

        l_builderStack.push(l_operation.m_ntreeIndex);
        l_tokenStack.push(l_operation);
    }//File while

    if(l_nOpenB < l_nClosedB)
        l_signal=9;
    if(l_nOpenB>l_nClosedB)
        l_signal=10;
    if(l_signal==4 && l_builderStack.size()==1 && l_tokenStack.size()==1)
        return true;
    if(l_tokenStack.size() > 1 && l_signal==4)
        l_signal=1;
    if(l_builderStack.size() != 1 && l_signal==4)
        l_signal=3;

    const QString l_signalArray[12]={"Success","Closed Brackets Missed", "Invalid Symbol",
                                     "Unexpected EOF","EOF found","Not enough tokens",
                                     "Not enough binary tokens","Invalid order of operators",
                                     "Too many closed brackets in the proposition",
                                     "Mismatch Brackets-Open Brackets Missed",
                                     "Mismatch Brackets- Closed Brackets Missed",
                                     "Over semi colons characters"};

    qDebug()<<l_signalArray[l_signal]<<"1. The error was found in row: "<<l_nErrorRow+1<<"\t Column:"<<
              l_nErrorCol;
    return false;
}
