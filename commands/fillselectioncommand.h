#ifndef FILLSELECTIONCOMMAND_H
#define FILLSELECTIONCOMMAND_H

#include "mapscene.h"

#include <QUndoCommand>

class FillSelectionCommand : public QUndoCommand
{
    using ItemsSelected = std::vector<std::tuple<TileItem*, QPen>>;

public:
    FillSelectionCommand(MapScene *mapScene,
                         const QString& textureName,
                         const ItemsSelected& itemsSelected,
                         QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    MapScene      *m_mapScene{nullptr};
    QString        m_textureName{""};
    ItemsSelected  m_itemsSelected{};
};

#endif // FILLSELECTIONCOMMAND_H
