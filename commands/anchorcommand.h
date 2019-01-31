#ifndef ANCHORCOMMAND_H
#define ANCHORCOMMAND_H

#include "mapscene.h"
#include "tileitem.h"

#include <map>

#include <QUndoCommand>
#include <QList>

class AnchorCommand : public QUndoCommand
{
public:
    AnchorCommand(MapScene *mapScene,
                  QList<TileItem*> *pastedTiles,
                  QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    int indexByPos(TileItem *item) const;
private:
    MapScene *m_mapScene{nullptr};
    QList<TileItem*> *m_pastedTiles{nullptr};
    QList<std::tuple<TileItem*, QString>> m_removedPastedTiles{};
    QList<std::tuple<TileItem*, QPointF>> m_firstAnchorPos{};
};

#endif // ANCHORCOMMAND_H
