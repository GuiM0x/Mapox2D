#include "anchorcommand.h"

AnchorCommand::AnchorCommand(MapScene *mapScene,
                             QList<TileItem*> *floatSelection,
                             QUndoCommand *parent)
    : QUndoCommand{parent},
      m_mapScene{mapScene},
      m_floatSelectionFromView{floatSelection}
{

    assert(m_mapScene != nullptr &&
            m_floatSelectionFromView != nullptr &&
            !m_floatSelectionFromView->empty());

    // New item are created to get independant from floatSelection
    for(const auto& item : *m_floatSelectionFromView){
        if(!item->name().isEmpty()){
            TileItem *tile = TileItem::copy(item);
            m_floatSelectionSaved.push_back(tile);
        }
    }

    QString infos = "Anchor Selection\nanchor selection";
    setText(infos);
}

void AnchorCommand::undo()
{
    m_floatSelectionFromView->clear();
    for(const auto& floatItem : m_floatSelectionSaved){
        m_floatSelectionFromView->push_back(floatItem);
        m_mapScene->addItem(floatItem);
    }

    for(const auto& oldItem : m_oldItemOnMap){
        const int index = oldItem->index();
        const QString textureName = oldItem->name();
        m_mapScene->fillTile(index, textureName);
    }
}

void AnchorCommand::redo()
{
    for(const auto& item : *m_floatSelectionFromView){
        m_mapScene->removeItem(item);
    }
    m_floatSelectionFromView->clear();

    m_oldItemOnMap.clear();
    for(const auto& floatItem : m_floatSelectionSaved){
        const int index = indexByPos(floatItem);
        const auto oldItem = TileItem::copy(m_mapScene->itemByIndex(index));
        m_oldItemOnMap.push_back(oldItem);
        if(floatItem->name() != oldItem->name())
            m_mapScene->fillTile(index, floatItem->name());
    }

    m_mapScene->triggerTool(true, ToolType::Brush);
}

int AnchorCommand::indexByPos(TileItem *item) const
{
    const QPointF pos = item->scenePos();
    const int index_x = static_cast<int>(pos.x()) / m_mapScene->tileWidth();
    const int index_y = static_cast<int>(pos.y()) / m_mapScene->tileHeight();
    const int index = (index_y * m_mapScene->cols()) + index_x;
    return index;
}
