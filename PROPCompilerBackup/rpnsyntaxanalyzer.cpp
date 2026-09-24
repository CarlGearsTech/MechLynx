#include "rpnsyntaxanalyzer.h"
#include <QStack>
#include <QDebug>

bool RPNSyntaxAnalyzer::Compile()const{
    QStack<int> l_builderStack;
    l_builderStack.clear();
    const QString l_signalTable[6]={"Success","Not enough parameters","Not implemeted operator",
                              "Invalid Symbol","End of File found", "Unexpected Error"};

    int l_nSignal=0;
    while(l_nSignal){
        auto L=m_pLA->getToken();

        switch(L.getType())
        {
        case ID:
            l_builderStack.push(m_pTB->atom(L.getToken()));
            break;
        case OPERATOR:
            if(l_builderStack.size()>=2 && L.getToken() != "!")
            {
                int right=l_builderStack.pop();
                int left=l_builderStack.pop();

                if(L.getToken() == "->")
                    l_builderStack.push(m_pTB->If(left,right));
                else if(L.getToken() == "|")
                    l_builderStack.push(m_pTB->Or(left,right));
                else if(L.getToken() == "&")
                    l_builderStack.push(m_pTB->And(left,right));
                else //bi implication
                    l_builderStack.push(m_pTB->Iff(left,right));
            }//Binary op if
            else if(l_builderStack.size()>=1 && L.getToken() == "!"){
                int left=l_builderStack.pop();
                l_builderStack.push(m_pTB->Not(left));
            }
            else
                l_nSignal=2;
            break;
        case ENDOFF:
            l_nSignal=4;
            break;
        case ERROR:
            l_nSignal=1;
            break;
        case OPENBRACKET:
        case CLOSEDBRAKET:
        default:
            l_nSignal=3;
            break;
        }//end switch
    }//end while

    if(l_nSignal == 4 && l_builderStack.size()==1)
        return true;
    else if(l_nSignal == 4 && l_builderStack.size() != 1)
        l_nSignal=5;
    qDebug()<<l_signalTable[l_nSignal];
    return false;
}

