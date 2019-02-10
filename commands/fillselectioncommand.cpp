#include "fillselectioncommand.h"

FillSelectionCommand::FillSelectionCommand(MapScene *mapScene,
                                           const QString& textureName,
                                           const ItemsSelected& itemsSelected,
                                           QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    assert(m_mapScene != nullptr);
    m_textureName = textureName;
    m_itemsSelected = itemsSelected;
    // If no selection, the map will be full filled
    if(m_itemsSelected.empty()){
        for(int i = 0; i < (m_mapScene->rows() * m_mapScene->cols()); ++i){
            m_itemsSelected.push_back(std::make_pair(m_mapScene->itemByIndex(i), QPen{}));
        }
    }
    const QString infos = "Selection filled with [" + m_textureName + "]";
    setText(infos);
}

void FillSelectionCommand::undo()
{
    for(const auto& item : m_itemsSelected){
        m_mapScene->removeLastLayer(std::get<0>(item)->index());
    }
}

void FillSelectionCommand::redo()
{
    for(const auto& item : m_itemsSelected){
        m_mapScene->fillTile(std::get<0>(item)->index(), m_textureName);
    }
}
