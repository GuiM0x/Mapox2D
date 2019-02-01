#include "deleteselectioncommand.h"

DeleteSelectionCommand::DeleteSelectionCommand(MapScene *mapScene,
                                               ItemsSelected *itemsSelected,
                                               QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_tilesSelected = itemsSelected;
    assert(mapScene != nullptr && m_tilesSelected != nullptr);
    QString infos = "Selection deleted\ndelete selection";
    setText(infos);
    for(const auto& it : *m_tilesSelected){
        auto item = std::get<0>(it);
        const QPen pen = std::get<1>(it);
        const QString tileName = item->name();
        const auto tuple = std::make_tuple(item, pen, tileName);
        m_tilesRemoved.push_back(tuple);
    }
}

void DeleteSelectionCommand::undo()
{
    for(const auto& it : m_tilesRemoved){
        auto item = std::get<0>(it);
        const QPen pen = std::get<1>(it);
        item->setPen(pen);
        m_mapScene->fillTile(item->index(), std::get<2>(it));
        m_tilesSelected->push_back(std::make_tuple(item, pen));
    }
}

void DeleteSelectionCommand::redo()
{
    for(const auto& it : m_tilesRemoved){
        auto item = std::get<0>(it);
        item->setOpacity(1);
        m_mapScene->deleteTile(item->index());
        const QPen pen = std::get<1>(it);
        item->setPen(pen);
    }
    m_tilesSelected->clear();
}
