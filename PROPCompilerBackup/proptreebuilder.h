#ifndef PROPTREEBUILDER_H
#define PROPTREEBUILDER_H

#include <QVector>
#include <QString>
#include <QMap>
#include "propnode.h"

class PROPTreeBuilder;
class PROPNodeFactory;

class PROPTree
{
friend class PROPTreeBuilder;
friend class PROPNodeFactory;
public:
    PROPTree() : _treeLst{}, _symblLst{}, _mapSymbol2Entry{}, _mapEntry2Symbol{} {}
private:
    QVector<PROPNode> _treeLst;
    QVector<QString> _symblLst;
    QMap<QString,int> _mapSymbol2Entry;
    QMap<int,QString> _mapEntry2Symbol;
};

class PROPNodeFactory
{
public:
static int create(
    PROPTree &tree,
    NodeType type,
    int first = -1,
    int second = -1);

static int createByName(
    PROPTree &tree,
    NodeType type,
    const QString &str = {});
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
public:
    int atom(QString str) {return PROPNodeFactory::createByName(_tree, NodeType::ATOM, str);}
    int Not(int first) {return PROPNodeFactory::create(_tree, NodeType::NOT, first);}
    int If(int first, int second) {return PROPNodeFactory::create(_tree, NodeType::IF, first, second);}
    int And(int first, int second) {return PROPNodeFactory::create(_tree, NodeType::AND, first, second);}
    int Iff(int first,int second) {return PROPNodeFactory::create(_tree, NodeType::IFF, first, second);}
    int Or(int first,int second) {return PROPNodeFactory::create(_tree, NodeType::OR, first, second);}
    int True() {return PROPNodeFactory::create(_tree, NodeType::TRUE);}
    PROPTreeBuilder();
    [[nodiscard]] QString getSymbolAt(qsizetype idx )const;
    [[nodiscard]] PROPNode getNodeAt(qsizetype  idx)const;
    [[nodiscard]] qsizetype getTreeSize()const;
    [[nodiscard]] QString getSymbolFromEntry(int entry)const;
    [[nodiscard]] QMap<QString,int> getMapSymbolEntry()const;
private:
    PROPTree _tree;
};

#endif // PROPTREEBUILDER_H
