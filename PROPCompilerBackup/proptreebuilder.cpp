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

int PROPTreeBuilder::atom(QString strId){
    QMap<QString,int>::const_iterator it=_mapSymbol2Entry.find(strId);

    if(it != _mapSymbol2Entry.end())
        return it.value();

    else{
        _mapSymbol2Entry.insert(strId,_treeLst.size());
        _mapEntry2Symbol.insert(_treeLst.size(),strId);
        _treeLst.append(PROPNode(PROPNode::ATOM,_symblLst.size(),-1));
        _symblLst.push_back(strId);
        return _treeLst.size()-1;
    }
}

int PROPTreeBuilder::valueAtomBySymbol(QString symbol){
    if(_mapSymbol2Entry.contains(symbol))
        return _mapSymbol2Entry.value(symbol);
    return -1;
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
