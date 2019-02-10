#ifndef DELETETILECOMMAND_H
#define DELETETILECOMMAND_H

#include "mapscene.h"

#include <QUndoCommand>

class DeleteTileCommand : public QUndoCommand
{
public:
    explicit DeleteTileCommand(MapScene* mapScene,
                               QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    MapScene   *m_mapScene{nullptr};
    int         m_tileIndex{};
    TileItem   *m_tileRemoved{};
};

#endif // DELETETILECOMMAND_H
