#ifndef FILLTILECOMMAND_H
#define FILLTILECOMMAND_H

#include "mapscene.h"
#include "tileitem.h"

#include <QUndoCommand>

class FillTileCommand : public QUndoCommand
{
public:
    FillTileCommand(MapScene *mapScene, QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    MapScene    *m_mapScene{nullptr};
    QString      m_textureInList{};
    int          m_tileIndex{};
};

#endif // FILLTILECOMMAND_H
