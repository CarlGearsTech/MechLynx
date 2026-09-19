#ifndef PROPTREEBUILDER_H
#define PROPTREEBUILDER_H

#include <QVector>
#include <QString>
#include <QMap>
#include "propnode.h"

class PROPTreeBuilder
{
private:
    QVector<PROPNode> _treeLst;
    QVector<QString> _symblLst;
    QMap<QString,int> _mapSymbol2Entry;
    QMap<int,QString> _mapEntry2Symbol;
public:
    PROPTreeBuilder();
    int atom(QString string);
    int Not(int first);
    int If(int first, int second);
    int And(int first, int second);
    int Iff(int first,int second);
    int Or(int first,int second);
    int True();
    [[nodiscard]] QString getSymbolAt(qsizetype idx )const;
    [[nodiscard]] PROPNode getNodeAt(qsizetype  idx)const;
    [[nodiscard]] qsizetype getTreeSize()const { return _treeLst.size();}
    [[nodiscard]] QString getSymbolFromEntry(int entry)const { return _mapEntry2Symbol.value(entry);}
    [[nodiscard]] QMap<QString,int> getMapSymbolEntry()const{return _mapSymbol2Entry;}
    int valueAtomBySymbol(QString symbol);
};
#endif // PROPTREEBUILDER_H
