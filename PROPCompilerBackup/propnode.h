#ifndef PROPNODE_H
#define PROPNODE_H

class PROPNode
{
public:
    enum NodeType{ATOM,NOT,IF,IFF,AND,OR,TRUE};
    PROPNode(NodeType type, int first, int second);
    int first()const{return m_nFirst;}
    int second()const{return m_nSecond;}
    NodeType getType()const{return m_nodeType;}
    ~PROPNode();
private:
    NodeType m_nodeType;
    int m_nFirst;
    int m_nSecond;
};

#endif // PROPNODE_H
