#ifndef PROPTREEBUILDER_H
#define PROPTREEBUILDER_H

#include <QVector>
#include <QString>
#include <QMap>
#include "propnode.h"
#include "proptree.h"
#include "propnodefactory.h"

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
    PROPTreeBuilder();
    [[nodiscard]] int addAtomNode(QString str) { return PROPNodeFactory::createByName(_tree, NodeType::ATOM, str); }
    [[nodiscard]] int addNotNode(int first) { return PROPNodeFactory::create(_tree, NodeType::NOT, first); }
    [[nodiscard]] int addIfNode(int first, int second) { return PROPNodeFactory::create(_tree, NodeType::IF, first, second); }
    [[nodiscard]] int addAndNode(int first, int second) { return PROPNodeFactory::create(_tree, NodeType::AND, first, second); }
    [[nodiscard]] int addIffNode(int first, int second) { return PROPNodeFactory::create(_tree, NodeType::IFF, first, second); }
    [[nodiscard]] int addOrNode(int first, int second) { return PROPNodeFactory::create(_tree, NodeType::OR, first, second); }
    [[nodiscard]] int addTrueNode() { return PROPNodeFactory::create(_tree, NodeType::TRUE); }
    [[nodiscard]] QString getSymbolAt(qsizetype idx) const;
    [[nodiscard]] PROPNode getNodeAt(qsizetype idx) const;
    [[nodiscard]] qsizetype getTreeSize() const;
    [[nodiscard]] QString getSymbolFromEntry(int entry) const;
    [[nodiscard]] int getEntryFromSymbol(const QString &str) const;
private:
    PROPTree _tree;
};

#endif // PROPTREEBUILDER_H