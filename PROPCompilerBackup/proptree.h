#ifndef PROPTREE_H
#define PROPTREE_H

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

#endif // PROPTREE_H
