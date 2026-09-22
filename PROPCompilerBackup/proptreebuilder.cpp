#include "proptreebuilder.h"

PROPTreeBuilder::PROPTreeBuilder() : _tree{} {}

PROPNode PROPTreeBuilder::getNodeAt(qsizetype idx) const
{
    /* Out of boundaries is checked by Qt.*/
    return _tree._treeLst.at(idx);
}

qsizetype PROPTreeBuilder::getTreeSize() const
{
    return _tree._treeLst.size();
}

QString PROPTreeBuilder::getSymbolFromEntry(int entry) const
{
    return _tree._mapEntry2Symbol.value(entry);
}

QMap<QString, int> PROPTreeBuilder::getMapSymbolEntry() const
{
    return _tree._mapSymbol2Entry;
}

QString PROPTreeBuilder::getSymbolAt(qsizetype idx) const
{
    /* Out of boundaries is checked by Qt.*/
    return _tree._symblLst.at(idx);
}

int PROPNodeFactory::createByName(
    PROPTree &tree,
    NodeType type,
    const QString &str)
{
    switch (type)
    {
        case NodeType::ATOM:
        {
            if (str.isEmpty())
                return -1;
            const auto it = tree._mapSymbol2Entry.find(str);
            if (it != tree._mapSymbol2Entry.end())
                return it.value();
            else
            {
                /* Inserting helpers containers.*/
                tree._mapSymbol2Entry.insert(str, tree._treeLst.size());
                tree._mapEntry2Symbol.insert(tree._treeLst.size(), str);
                /* TreeLst indexing based on all posible entries already inserted.*/
                tree._treeLst.append(PROPNode::make_atom(tree._symblLst.size()));
                tree._symblLst.push_back(str);
                /* Return index of new entry.*/
                return tree._treeLst.size() - 1;
            }
        }
        default:
        {
            throw std::runtime_error("Unknown PROPNodeType");
            return -1;
        }
    }
}

int PROPNodeFactory::create(
    PROPTree &tree,
    NodeType type,
    int first,
    int second)
{
    if(type >= MAX_NODE_TYPE)
    {
        throw std::runtime_error("Unknown PROPNodeType");
        return -1;
    }
    else
    {
        tree._treeLst.append(PROPNode(type, first, second));
        return tree._treeLst.size() - 1u;
    }
}
