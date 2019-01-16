#include "deletetilecommand.h"

DeleteTileCommand::DeleteTileCommand(MapScene* mapScene, QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_textureName = m_mapScene->currentTextureName();
    m_tileIndex = m_mapScene->currentTile();
    QString infos = "Texture [" + QString::number(m_tileIndex) +
                    "] texture [" + m_textureName + "] deleted";
    setText(infos);
}

void DeleteTileCommand::undo()
{
    m_mapScene->fillTile(m_tileIndex, m_textureName);
}

void DeleteTileCommand::redo()
{
    m_mapScene->deleteTile(m_tileIndex);
}
