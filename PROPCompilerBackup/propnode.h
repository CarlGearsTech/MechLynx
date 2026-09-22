#ifndef PROPNODE_H
#define PROPNODE_H

enum NodeType {ATOM,NOT,IF,IFF,AND,OR,TRUE, MAX_NODE_TYPE};

/**
 * @class PROPNode
 * @brief Represents the most atomic element of the tree builder.
 *
 * A PROPNode represents an operand or a logical expression composed of
 * operands and operators.
 *
 * The supported node structures are:
 * - ATOM
 * - IF, IFF, AND, OR:  <operand><operator><operand>
 * - NOT:               <operator><operand>
 *
 * A special TRUE node is used to mark the last node in the tree.
 */
class PROPNode
{
public:
    static PROPNode make_atom(int first)
    {
        return PROPNode(ATOM, first, -1);
    }
    PROPNode(NodeType type, int first, int second): _nodeType(type),_nFirst(first),_nSecond(second){}
    [[nodiscard]] int getFirst()const{return _nFirst;}
    [[nodiscard]] int getSecond()const{return _nSecond;}
    [[nodiscard]] NodeType getType()const{return _nodeType;}
private:
    NodeType _nodeType;
    int _nFirst;
    int _nSecond;
};

#endif // PROPNODE_H
