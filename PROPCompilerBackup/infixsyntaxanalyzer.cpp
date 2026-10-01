#include "infixsyntaxanalyzer.h"
#include <QDebug>

/* Construye el tree */
void InfixSyntaxAnalyzer::buildLeftToken(int &left,
                                         int &right,
                                         PropLexem &operation,
                                         unsigned int &signal,
                                         QStack<int> &builderStack,
                                         QStack<PropLexem> &tokenStack,
                                         bool &isValidated)
{
    /* NOT special case.*/
    if (operation.getToken() == "!")
    {
        right = builderStack.pop();
        operation.setTreeIdx(_treeBuilder.addNotNode(right));
    }
    else
    {
        if (!tokenStack.size())
        {
            signal = 6;
            return;
        }
        if (!builderStack.size())
        {
            signal = 7;
            return;
        }
        PropLexem leftToken = tokenStack.pop();

        switch (leftToken.getType())
        {
        case ID:
            builderStack.push(_treeBuilder.addAtomNode(leftToken.getToken()));
            left = builderStack.pop();
            right = builderStack.pop();
            isValidated = true;
            return;
        case OPERATOR:
            right = builderStack.pop();
            left = builderStack.pop();
            isValidated = true;
            break;
        case EOL:
        case OPENBRACKET:
        case CLOSEDBRAKET:
        case COMMENT:
        case ERROR:
        case ENDOFF:
        default:
            signal = 2;
            break;
        } // end left token switch
    }
}

bool InfixSyntaxAnalyzer::Compile()
{
    unsigned int signal = 0;
    QStack<PropLexem> tokenStack;
    tokenStack.clear();
    unsigned int numErrorRow = 0;
    unsigned int numErrorColumn = 0;
    bool isRuleAvailable = false;
    bool isFirstRule = true;
    QStack<int> builderStack;
    builderStack.clear();
    unsigned int closedBrackets = 0;
    unsigned int openBrackets = 0;
    bool isValidated = false;

    while (!signal)
    {
        while (!signal)
        {
            PropLexem L = _pLA->getToken();
            if (L.getType() == EOL)
            {
                ++numErrorRow;
                numErrorColumn = 0;
            }
            else if (L.getType() == ENDOFF)
            {
                signal = 4;
                break;
            }
            else
            {
                ++numErrorColumn;
                if (L.getToken() == ";")
                {
                    if (isFirstRule)
                    {
                        isFirstRule = false;
                        builderStack.push(_treeBuilder.addTrueNode());
                        tokenStack.push(_pLA->buildToken(OPERATOR, "True"));
                    }
                    isRuleAvailable = true;
                    break;
                }
                else if (L.getType() == COMMENT)
                {
                    ++numErrorRow;
                    continue;
                }
                else if (L.getType() == ERROR)
                {
                    signal = 2;
                    break;
                }
                else if (L.getToken() == ")")
                {
                    ++closedBrackets;
                    break;
                }
                else if (L.getToken() == "(")
                {
                    ++openBrackets;
                    continue;
                }
                tokenStack.push(L);
            } // end toke else
        } // Rule while

        int l_right = 0;
        int l_left = 0;

        // EOF signal
        if (signal == 4)
            break;

        // Too many semicolons
        if (isRuleAvailable && builderStack.size() < 2)
        {
            signal = 11;
            break;
        }

        // No enough tokens to fit the operations
        if (signal == 4 && tokenStack.size() < 2 && builderStack.size() != 1)
        {
            signal = 5;
            break;
        }

        // Ensure operation with 2 operands
        if (tokenStack.size() < 2 && builderStack.size() != 1)
        {
            signal = 8;
            break;
        }

        if (signal == 2)
            break;

        PropLexem l_rightToken = tokenStack.pop();
        PropLexem l_operation = tokenStack.pop();

        if (isRuleAvailable)
        {
            l_right = builderStack.pop();
            l_left = builderStack.pop();
            l_operation = _pLA->buildToken(OPERATOR, "&");
            l_operation.setTreeIdx(_treeBuilder.addAndNode(l_left, l_right));
            isRuleAvailable = false;
        }
        else
        {
            switch (l_rightToken.getType())
            {
            case ID:
                builderStack.push(_treeBuilder.addAtomNode(l_rightToken.getToken()));
                buildLeftToken(l_left, l_right, l_operation, signal, builderStack,
                               tokenStack, isValidated);
                break;
            case OPERATOR:
                buildLeftToken(l_left, l_right, l_operation, signal, builderStack,
                               tokenStack, isValidated);
                break;
            case EOL:
            case OPENBRACKET:
            case CLOSEDBRAKET:
            case ERROR:
            case COMMENT:
            case ENDOFF:
            default:
                signal = 2;
                break;
            } // end right token switch
            if (isValidated)
            {
                if (l_operation.getToken() == "->")
                    l_operation.setTreeIdx(_treeBuilder.addIfNode(l_left, l_right));
                else if (l_operation.getToken() == "&")
                    l_operation.setTreeIdx(_treeBuilder.addAndNode(l_left, l_right));
                else if (l_operation.getToken() == "|")
                    l_operation.setTreeIdx(_treeBuilder.addOrNode(l_left, l_right));
                else if (l_operation.getToken() == "<->")
                    l_operation.setTreeIdx(_treeBuilder.addIffNode(l_left, l_right));
                else
                    signal = 2;
                isValidated = false;
            }
        } // end else

        builderStack.push(l_operation.getTreeIdx());
        tokenStack.push(l_operation);
    } // File while

    bool retFlag;
    bool resultCompilation = determineResultCompilation(openBrackets, closedBrackets, signal, builderStack, tokenStack, retFlag);
    if (retFlag) return resultCompilation;

    const QVector<QString> errors = {"Success", "Closed Brackets Missed", "Invalid Symbol",
                                     "Unexpected EOF", "EOF found", "Not enough tokens",
                                     "Not enough binary tokens", "Invalid order of operators",
                                     "Too many closed brackets in the proposition",
                                     "Mismatch Brackets-Open Brackets Missed",
                                     "Mismatch Brackets- Closed Brackets Missed",
                                     "Over semi colons characters"};

    qDebug() << errors.at(signal) << "1. The error was found in row: " << numErrorRow + 1 << "\t Column:" << numErrorColumn;
    _isCompiled = false;
    return false;
}

bool InfixSyntaxAnalyzer::determineResultCompilation(unsigned int l_nOpenB, unsigned int l_nClosedB, unsigned int &signal, QStack<int> &l_builderStack, QStack<PropLexem> &l_tokenStack, bool &retFlag)
{
    retFlag = true;
    if (l_nOpenB < l_nClosedB)
        signal = 9;
    if (l_nOpenB > l_nClosedB)
        signal = 10;
    if (signal == 4 && l_builderStack.size() == 1 && l_tokenStack.size() == 1)
    {
        _isCompiled = true;
        return true;
    }
    if (l_tokenStack.size() > 1 && signal == 4)
        signal = 1;
    if (l_builderStack.size() != 1 && signal == 4)
        signal = 3;
    retFlag = false;
    return {};
}
