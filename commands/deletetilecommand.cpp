#include "deletetilecommand.h"

DeleteTileCommand::DeleteTileCommand(MapScene* mapScene,
                                     QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_textureName = m_mapScene->currentTileName();
    m_tileIndex = m_mapScene->currentTile();
    QString infos = "Texture [" + QString::number(m_tileIndex) +
                    "] texture [" + m_textureName + "] deleted";
    setText(infos);
}

void DeleteTileCommand::undo()
{
    assert(m_mapScene != nullptr && "DeleteTileCommand::m_mapScene cannot be null");
    m_mapScene->fillTile(m_tileIndex, m_textureName);
}

void DeleteTileCommand::redo()
{
    assert(m_mapScene != nullptr && "DeleteTileCommand::m_mapScene cannot be null");
    m_mapScene->deleteTile(m_tileIndex);
}
