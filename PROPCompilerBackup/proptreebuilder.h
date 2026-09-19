#ifndef PROPTREEBUILDER_H
#define PROPTREEBUILDER_H

#include <QVector>
#include <QString>
#include <QMap>
#include "propnode.h"

class PROPTreeBuilder
{
private:
    QVector<PROPNode> m_lstTree;
    QVector<QString> m_lstSymbol;
    QMap<QString,int> m_mapSymbol2Entry;
    QMap<int,QString> m_mapEntry2Symbol;
public:
    PROPTreeBuilder();
    int atom(QString string);
    int Not(int first);
    int If(int first, int second);
    int And(int first, int second);
    int Iff(int first,int second);
    int Or(int first,int second);
    int True();
    QVector<QString> getSymbols()const{return this->m_lstSymbol;}
    QVector<PROPNode> getTree()const{return this->m_lstTree;}
    QMap<int,QString> getMapEntrySymbol()const{return m_mapEntry2Symbol;}
    QMap<QString,int> getMapSymbolEntry()const{return m_mapSymbol2Entry;}
    int valueAtomBySymbol(QString symbol);
};

#endif // PROPTREEBUILDER_H
