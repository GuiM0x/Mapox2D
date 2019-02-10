#include "deletetilecommand.h"

DeleteTileCommand::DeleteTileCommand(MapScene* mapScene,
                                     QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    assert(m_mapScene != nullptr && "DeleteTileCommand::m_mapScene cannot be null");
    m_tileIndex = m_mapScene->currentTile();
    m_tileRemoved = TileItem::copy(m_mapScene->itemByIndex(m_tileIndex));
    QString infos = "Texture [" + QString::number(m_tileIndex) +
                    "] texture [" + m_tileRemoved->name() + "] deleted";
    setText(infos);
}

void DeleteTileCommand::undo()
{
    TileItem *tileOnMap = m_mapScene->itemByIndex(m_tileIndex);
    tileOnMap->copyLayers(m_tileRemoved->layers());
}

void DeleteTileCommand::redo()
{
    TileItem *tileOnMap = m_mapScene->itemByIndex(m_tileIndex);
    tileOnMap->clear();
}
