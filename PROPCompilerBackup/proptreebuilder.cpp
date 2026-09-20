#include "proptreebuilder.h"

PROPTreeBuilder::PROPTreeBuilder()
: _treeLst{}, _symblLst{}, _mapSymbol2Entry{}, _mapEntry2Symbol{}
{

}

PROPNode PROPTreeBuilder::getNodeAt(qsizetype  idx)const
{
    /* Out of boundaries is checked by Qt.*/
    return _treeLst.at(idx);
}

QString PROPTreeBuilder::getSymbolAt(qsizetype idx)const
{
    /* Out of boundaries is checked by Qt.*/
    return _symblLst.at(idx);
}

int PROPTreeBuilder::Not(int first){
    _treeLst.append(PROPNode(PROPNode::NOT,first,-1));
    return _treeLst.size()-1;
}

int PROPTreeBuilder::And(int first, int second){
    _treeLst.append(PROPNode(PROPNode::AND,first,second));
    return _treeLst.size()-1;
}

int PROPTreeBuilder::If(int first, int second){
    _treeLst.append(PROPNode(PROPNode::IF,first,second));
    return _treeLst.size()-1;
}

int PROPTreeBuilder::Iff(int first, int second){
    _treeLst.append(PROPNode(PROPNode::IFF,first,second));
    return _treeLst.size()-1;
}

int PROPTreeBuilder::Or(int first, int second){
    _treeLst.append(PROPNode(PROPNode::OR,first,second));
    return _treeLst.size()-1;
}

int PROPTreeBuilder::True(){
    _treeLst.append(PROPNode(PROPNode::TRUE,-1,-1));
    return _treeLst.size()-1;
}

/**
 * @brief Inserts an atom if it does not already exist in the tree.
 * @return The newly created node from the list.
 * @note The node also updates the maps and symbol list.
 */
int PROPTreeBuilder::atom(const QString &str)
{
    const auto it = _mapSymbol2Entry.find(str);
    if(it != _mapSymbol2Entry.end())
        return it.value();
    else
    {
        _mapSymbol2Entry.insert(str,_treeLst.size());
        _mapEntry2Symbol.insert(_treeLst.size(),str);
        _treeLst.append(PROPNode::make_atom(_symblLst.size()));
        _symblLst.push_back(str);
        return _treeLst.size()-1;
    }
}

int PROPNodeFactory::createAtom(const QString &string)
{
    return 0;
}

int PROPNodeFactory::createNot(int first)
{
    return 0;
}

int PROPNodeFactory::createIf(int first, int second)
{
    return 0;
}

int PROPNodeFactory::createAnd(int first, int second)
{
    return 0;
}

int PROPNodeFactory::createIff(int first, int second)
{
    return 0;
}

int PROPNodeFactory::createOr(int first, int second)
{
    return 0;
}

int PROPNodeFactory::createTrue()
{
    return 0;
}
