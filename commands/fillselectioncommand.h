#ifndef FILLSELECTIONCOMMAND_H
#define FILLSELECTIONCOMMAND_H

#include "mapscene.h"

#include <QUndoCommand>

class FillSelectionCommand : public QUndoCommand
{
    using ItemsSelected = std::vector<std::tuple<TileItem*, QPen>>;
    using OldItems = std::vector<std::tuple<TileItem*, QString>>;

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
    OldItems       m_oldItems{};
};

#endif // FILLSELECTIONCOMMAND_H
