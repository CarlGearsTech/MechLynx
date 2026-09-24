#include "propnodefactory.h"
#include "proptree.h"
#include "propnode.h"

/**
 * @brief Specialized interface for creating ATOM nodes by name and insertion to the instance tree.
 *
 * Other node types cannot be created through this interface because
 * they are not represented by string identifiers.
 */
qsizetype PROPNodeFactory::createByName(
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
            const auto entry = tree._treeLst.size();
            /* Inserting helpers containers.*/
            tree._mapSymbol2Entry.insert(str, entry);
            tree._mapEntry2Symbol.insert(entry, str);
            /* TreeLst indexing based on all posible entries already inserted.*/
            tree._treeLst.append(PROPNode::make_atom(tree._symblLst.size()));
            tree._symblLst.push_back(str);
            /* Return index of new entry.*/
            return entry;
        }
        default:
        {
            throw std::runtime_error("Unknown PROPNodeType");
        }
    }
}

/**
 * @brief Specialized interface for creating all node types except ATOM nodes
 *        by name and inserting them into the instance tree.
 */
qsizetype PROPNodeFactory::create(
    PROPTree &tree,
    NodeType type,
    int first,
    int second)
{
    if (type >= MAX_NODE_TYPE)
        throw std::runtime_error("Unknown PROPNodeType");
    const auto entry = tree._treeLst.size();
    tree._treeLst.append(PROPNode(type, first, second));
    return entry;
}
