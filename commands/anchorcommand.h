#ifndef ANCHORCOMMAND_H
#define ANCHORCOMMAND_H

#include "mapscene.h"
#include "tileitem.h"

#include <QUndoCommand>
#include <QList>

class AnchorCommand : public QUndoCommand
{
public:
    AnchorCommand(MapScene *mapScene,
                  QList<TileItem*> *floatSelection,
                  QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    int indexByPos(TileItem *item) const;
private:
    MapScene               *m_mapScene{nullptr};
    QList<TileItem*>       *m_floatSelectionFromView{nullptr};
    QList<TileItem*>        m_copiedSelection{};
    QList<TileItem*>        m_copiedItemOnMap{};
};

#endif // ANCHORCOMMAND_H
