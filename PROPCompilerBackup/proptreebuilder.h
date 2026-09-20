#ifndef PROPTREEBUILDER_H
#define PROPTREEBUILDER_H

#include <QVector>
#include <QString>
#include <QMap>
#include "propnode.h"

class PROPNodeFactory
{
public:
    static int create(
        PROPNode::NodeType type,
        const QString& string = {},
        int first = -1,
        int second = -1)
    {
        switch (type)
        {
        case PROPNode::NodeType::ATOM:
            return createAtom(string);

        case PROPNode::NodeType::NOT:
            return createNot(first);

        case PROPNode::NodeType::IF:
            return createIf(first, second);

        case PROPNode::NodeType::AND:
            return createAnd(first, second);

        case PROPNode::NodeType::IFF:
            return createIff(first, second);

        case PROPNode::NodeType::OR:
            return createOr(first, second);

        case PROPNode::NodeType::TRUE:
            return createTrue();
        default:
            throw std::runtime_error("Unknown PROPNodeType");
        }
    }
private:
    static int createAtom(const QString& string);
    static int createNot(int first);
    static int createIf(int first, int second);
    static int createAnd(int first, int second);
    static int createIff(int first, int second);
    static int createOr(int first, int second);
    static int createTrue();
};

/**

* @brief Manages PROP nodes of the different supported types.
*
* Provides helper functionality for creating and managing PROP nodes, as well
* as an interface for other components that need to access information about
* the resulting tree.
*
* Supports node insertion and provides the necessary operations to interact
* with the PROP tree.
  */
class PROPTreeBuilder
{
friend class PROPNodeFactory;
private:
    QVector<PROPNode> _treeLst;
    QVector<QString> _symblLst;
    QMap<QString,int> _mapSymbol2Entry;
    QMap<int,QString> _mapEntry2Symbol;
public:
    int atom(const QString& str);   
    //int atom(QString string) {PROPNodeFactory::create(PROPNode::NodeType::ATOM,string);}
    int Not(int first);
    int If(int first, int second);
    int And(int first, int second);
    int Iff(int first,int second);
    int Or(int first,int second);
    int True();
    PROPTreeBuilder();
    [[nodiscard]] QString getSymbolAt(qsizetype idx )const;
    [[nodiscard]] PROPNode getNodeAt(qsizetype  idx)const;
    [[nodiscard]] qsizetype getTreeSize()const { return _treeLst.size();}
    [[nodiscard]] QString getSymbolFromEntry(int entry)const { return _mapEntry2Symbol.value(entry);}
    [[nodiscard]] QMap<QString,int> getMapSymbolEntry()const{return _mapSymbol2Entry;}
};

#endif // PROPTREEBUILDER_H
