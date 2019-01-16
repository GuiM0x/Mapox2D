#ifndef FILLTILECOMMAND_H
#define FILLTILECOMMAND_H

#include "mapscene.h"

#include <QUndoCommand>

class FillTileCommand : public QUndoCommand
{
public:
    FillTileCommand(MapScene *mapScene, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    MapScene *m_mapScene{nullptr};
    QString   m_textureName{};
    int       m_tileIndex{};
};

#endif // FILLTILECOMMAND_H
