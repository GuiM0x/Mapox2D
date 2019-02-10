#include "deleteselectioncommand.h"

DeleteSelectionCommand::DeleteSelectionCommand(MapScene *mapScene,
                                               const ItemsSelected *itemsSelected,
                                               QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    assert(mapScene != nullptr && itemsSelected != nullptr);
    QString infos = "Selection deleted\ndelete selection";
    setText(infos);
    for(const auto& item : *itemsSelected){
        m_tilesRemoved.push_back(TileItem::copy(std::get<0>(item)));
    }
}

void DeleteSelectionCommand::undo()
{
    for(const auto& item : m_tilesRemoved){
        TileItem *tile = m_mapScene->itemByIndex(item->index());
        tile->addLayer(item->name(), item->brush());
    }
}

void DeleteSelectionCommand::redo()
{
    for(const auto& item : m_tilesRemoved){
        m_mapScene->deleteTile(item->index());
    }
}
