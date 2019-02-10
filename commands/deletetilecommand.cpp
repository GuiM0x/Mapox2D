#include "deletetilecommand.h"

DeleteTileCommand::DeleteTileCommand(MapScene* mapScene,
                                     QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    assert(m_mapScene != nullptr && "DeleteTileCommand::m_mapScene cannot be null");
    m_textureName = m_mapScene->currentTileName();
    m_tileIndex = m_mapScene->currentTile();
    TileItem *tile = m_mapScene->itemByIndex(m_tileIndex);
    m_brush = tile->brush();
    QString infos = "Texture [" + QString::number(m_tileIndex) +
                    "] texture [" + m_textureName + "] deleted";
    setText(infos);
}

void DeleteTileCommand::undo()
{
    TileItem *tile = m_mapScene->itemByIndex(m_tileIndex);
    tile->addLayer(m_textureName, m_brush);
}

void DeleteTileCommand::redo()
{
    m_mapScene->deleteTile(m_tileIndex);
}
