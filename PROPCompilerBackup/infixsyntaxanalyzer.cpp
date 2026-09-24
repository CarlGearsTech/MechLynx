#include "infixsyntaxanalyzer.h"
#include <QDebug>

void InfixSyntaxAnalyzer::buildLeftToken(int &left,
                                         int &right,
                                         PropLexem &operation,
                                         unsigned int &signal,
                                         QStack<int> &builderStack,
                                         QStack<PropLexem> &tokenStack,
                                         bool &isValidated) const
{

    if(operation.getToken() == "!")
    {
        right=builderStack.pop();
        operation.setTreeIdx(m_pTB->Not(right));
    }
    else
    {
        if(!tokenStack.size())
        {
            signal=6;
            return;
        }
        if(!builderStack.size()){
            signal=7;
            return;
        }
        PropLexem leftToken=tokenStack.pop();

        switch (leftToken.getType()) 
        {
        case ID:
            builderStack.push(m_pTB->atom(leftToken.getToken()));
            left=builderStack.pop();
            right=builderStack.pop();
            isValidated=true;
            return;
        case OPERATOR:
            right=builderStack.pop();
            left=builderStack.pop();
            isValidated=true;
            break;
        case EOL:
        case OPENBRACKET:
        case CLOSEDBRAKET:
        case COMMENT:
        case ERROR:
        case ENDOFF:
        default:
            signal=2;
            break;
        }//end left token switch
    }
}

bool InfixSyntaxAnalyzer::Compile()const
{
    unsigned int l_signal=0;
    QStack<PropLexem> l_tokenStack;
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

    while(!l_signal)
    {

        while(!l_signal)
        {
            PropLexem L=m_pLA->getToken();
            if(L.getType() == EOL)
            {
                ++l_nErrorRow;
                l_nErrorCol=0;
            }
            else if(L.getType() == ENDOFF)
            {
                l_signal=4;
                break;
            }
            else
            {
                ++l_nErrorCol;

                if(L.getToken() == ";"){
                    if(l_isFirstRule){
                        l_isFirstRule=false;
                        l_builderStack.push(m_pTB->True());
                        l_tokenStack.push(m_pLA->buildToken(OPERATOR, "True"));
                    }
                    l_isRuleAvailable=true;
                    break;
                }
                else if(L.getType() == COMMENT)
                {
                    ++l_nErrorRow;
                    continue;
                }
                else if(L.getType() == ERROR)
                {
                    l_signal=2;
                    break;
                }
                else if(L.getToken() == ")")
                {
                    ++l_nClosedB;
                    break;
                }
                else if(L.getToken() == "(")
                {
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
        if(l_isRuleAvailable && l_builderStack.size()<2)
        {
            l_signal=11;
            break;
        }

        //No enough tokens to fit the operations
        if(l_signal==4 && l_tokenStack.size()<2 && l_builderStack.size() != 1)
        {
            l_signal=5;
            break;
        }

        //Ensure operation with 2 operands
        if(l_tokenStack.size()<2 && l_builderStack.size() !=1)
        {
            l_signal=8;
            break;
        }

        if(l_signal==2)
            break;

        PropLexem l_rightToken=l_tokenStack.pop();
        PropLexem l_operation= l_tokenStack.pop();

        if(l_isRuleAvailable){
            l_right=l_builderStack.pop();
            l_left=l_builderStack.pop();
            l_operation=m_pLA->buildToken(OPERATOR,"&");
            l_operation.setTreeIdx(m_pTB->And(l_left,l_right));
            l_isRuleAvailable=false;
        }
        else{
            switch (l_rightToken.getType()) 
            {
            case ID:
                l_builderStack.push(m_pTB->atom(l_rightToken.getToken()));
                buildLeftToken(l_left,l_right,l_operation,l_signal,l_builderStack,
                               l_tokenStack,l_isValidated);
                break;
            case OPERATOR:
                buildLeftToken(l_left,l_right,l_operation,l_signal,l_builderStack,
                               l_tokenStack,l_isValidated);
                break;
            case EOL:
            case OPENBRACKET:
            case CLOSEDBRAKET:
            case ERROR:
            case COMMENT:
            case ENDOFF:
            default:
                l_signal=2;
                break;
            }//end right token switch
            if(l_isValidated)
            {
                if(l_operation.getToken()=="->")
                    l_operation.setTreeIdx(m_pTB->If(l_left,l_right));
                else if(l_operation.getToken() == "&")
                    l_operation.setTreeIdx(m_pTB->And(l_left,l_right));
                else if(l_operation.getToken() == "|")
                    l_operation.setTreeIdx(m_pTB->Or(l_left,l_right));
                else if(l_operation.getToken() == "<->")
                    l_operation.setTreeIdx(m_pTB->Iff(l_left,l_right));
                else
                    l_signal=2;
                l_isValidated=false;
            }
        }//end else

        l_builderStack.push(l_operation.getTreeIdx());
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

    const QVector<QString> errors ={"Success","Closed Brackets Missed", "Invalid Symbol",
                                     "Unexpected EOF","EOF found","Not enough tokens",
                                     "Not enough binary tokens","Invalid order of operators",
                                     "Too many closed brackets in the proposition",
                                     "Mismatch Brackets-Open Brackets Missed",
                                     "Mismatch Brackets- Closed Brackets Missed",
                                     "Over semi colons characters"};

    
    qDebug()<<errors.at(l_signal)<<"1. The error was found in row: "<<l_nErrorRow+1<<"\t Column:"<<
              l_nErrorCol;
    return false;
}
