#ifndef DELETESELECTIONCOMMAND_H
#define DELETESELECTIONCOMMAND_H

#include "tileitem.h"
#include "mapscene.h"

#include <QUndoCommand>

class DeleteSelectionCommand : public QUndoCommand
{
    using ItemsSelected = std::vector<std::tuple<TileItem*, QPen>>;
    using ItemsRemoved  = std::vector<TileItem*>;

public:
    DeleteSelectionCommand(MapScene *mapScene,
                           const ItemsSelected *itemsSelected,
                           QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    MapScene      *m_mapScene{nullptr};
    ItemsRemoved   m_tilesRemoved{};
};

#endif // DELETESELECTIONCOMMAND_H
