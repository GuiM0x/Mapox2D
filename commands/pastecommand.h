#ifndef PASTECOMMAND_H
#define PASTECOMMAND_H

#include "mapscene.h"
#include "tileitem.h"

#include <QUndoCommand>
#include <QList>

class PasteCommand : public QUndoCommand
{
public:
    PasteCommand(MapScene* scene,
                 const QList<TileItem*>& copiedTiles,
                 QList<TileItem*> *pastedTiles,
                 QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    MapScene *m_mapScene{nullptr};
    QList<TileItem*>  m_copiedTiles{};
    QList<TileItem*> *m_pastedTiles{nullptr};
};

#endif // PASTECOMMAND_H
