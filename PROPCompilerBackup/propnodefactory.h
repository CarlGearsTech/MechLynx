#ifndef PROPNODEFACTORY_H
#define PROPNODEFACTORY_H

#include <QString>
#include "propnode.h"
class PROPTree;

class PROPNodeFactory
{
public:
    [[nodiscard]] static qsizetype create( PROPTree &tree, NodeType type, int first = -1, int second = -1);
    [[nodiscard]] static qsizetype createByName( PROPTree &tree, NodeType type, const QString &str = {});
};
#endif // PROPNODEFACTORY_H
