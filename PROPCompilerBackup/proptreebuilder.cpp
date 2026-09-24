#include "proptreebuilder.h"

PROPTreeBuilder::PROPTreeBuilder() : _tree{} {}

/**
 * @brief Retrieves a node from the tree at the specified index.
 *
 * @param idx Index of the node to retrieve.
 * @return The node stored at the specified index.
 *
 * @note Qt performs bounds checking on the requested index.
 */
PROPNode PROPTreeBuilder::getNodeAt(qsizetype idx) const
{
    /* Out of boundaries is checked by Qt.*/
    return _tree._treeLst.at(idx);
}

/**
 * @brief Returns the number of nodes currently stored in the tree.
 *
 * @return The number of nodes in the tree.
 */
qsizetype PROPTreeBuilder::getTreeSize() const
{
    return _tree._treeLst.size();
}

/**
 * @brief Retrieves the symbol associated with a node entry.
 *
 * @param entry Entry identifier used to look up the corresponding symbol.
 * @return The symbol associated with the specified entry.
 *
 * @note Returns an empty string if the entry is not present in the map.
 */
QString PROPTreeBuilder::getSymbolFromEntry(int entry) const
{
    return _tree._mapEntry2Symbol.value(entry);
}

/**
 * @brief Retrieves the entry identifier associated with a symbol.
 *
 * @param str Symbol used to look up the corresponding entry.
 * @return The entry identifier associated with the specified symbol.
 *
 * @note Returns the default value for the map's value type if the symbol
 *       is not present.
 */
int PROPTreeBuilder::getEntryFromSymbol(const QString &str) const
{
    return _tree._mapSymbol2Entry.value(str);
}

/**
 * @brief Retrieves the symbol stored at the specified index.
 *
 * @param idx Index of the symbol to retrieve.
 * @return The symbol stored at the specified index.
 *
 * @note Qt performs bounds checking on the requested index.
 */
QString PROPTreeBuilder::getSymbolAt(qsizetype idx) const
{
    return _tree._symblLst.at(idx);
}
