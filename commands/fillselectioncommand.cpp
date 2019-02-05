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
    if(m_itemsSelected.empty()){ // Necessary ?
        for(int i = 0; i < (m_mapScene->rows() * m_mapScene->cols()); ++i){
            m_itemsSelected.push_back(std::make_pair(m_mapScene->itemByIndex(i), QPen{}));
        }
    }
    for(const auto& item : m_itemsSelected){
        const QString oldName = std::get<0>(item)->name();
        m_oldItems.push_back(std::make_pair(std::get<0>(item), oldName));
    }
    const QString infos = "Selection filled with [" + m_textureName + "]";
    setText(infos);
}

void FillSelectionCommand::undo()
{
    for(const auto& item : m_oldItems){
        m_mapScene->deleteTile(std::get<0>(item)->index());
        m_mapScene->fillTile(std::get<0>(item)->index(), std::get<1>(item), true);
    }
}

void FillSelectionCommand::redo()
{
    for(const auto& item : m_itemsSelected){
        m_mapScene->fillTile(std::get<0>(item)->index(), m_textureName, true);
    }
}
