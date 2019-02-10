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
    TileItem *tile = m_mapScene->itemByIndex(m_tileIndex);
    tile->addLayer(m_tileRemoved->name(), m_tileRemoved->brush(), true);
}

void DeleteTileCommand::redo()
{
    TileItem *tile = m_mapScene->itemByIndex(m_tileIndex);
    tile->removeLastLayer();
}
