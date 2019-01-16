#include "filltilecommand.h"

FillTileCommand::FillTileCommand(MapScene *mapScene, QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_textureName = m_mapScene->currentTextureName();
    m_tileIndex = m_mapScene->currentTile();
    QString infos = "Tile [" + QString::number(m_tileIndex) +
                    "] texture [" + m_textureName + "] added";
    setText(infos);
}

void FillTileCommand::undo()
{
    m_mapScene->deleteTile(m_tileIndex);
}

void FillTileCommand::redo()
{
    m_mapScene->fillTile(m_tileIndex, m_textureName);
}
