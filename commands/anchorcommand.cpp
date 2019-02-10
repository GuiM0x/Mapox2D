#include "anchorcommand.h"

AnchorCommand::AnchorCommand(MapScene *mapScene,
                             QList<TileItem*> *floatSelection,
                             QUndoCommand *parent)
    : QUndoCommand{parent},
      m_mapScene{mapScene},
      m_floatSelectionFromView{floatSelection}
{
    // Warning : don't use index of float item
    //           -> use index by pos instead

    assert(m_mapScene != nullptr &&
            m_floatSelectionFromView != nullptr &&
            !m_floatSelectionFromView->empty());

    QString infos = "Anchor Selection\nanchor selection";
    setText(infos);

    for(const auto& item : *m_floatSelectionFromView){
        // Selection copied
        m_copiedSelection.push_back(TileItem::copy(item));
        // Item on map copied
        const int index = indexByPos(item);
        TileItem *copy = TileItem::copy(m_mapScene->itemByIndex(index));
        m_copiedItemOnMap.push_back(copy);
    }
}

void AnchorCommand::undo()
{
    // Retrieve float selection
    m_floatSelectionFromView->clear();
    for(const auto& copy : m_copiedSelection){
        m_floatSelectionFromView->push_back(copy);
        m_mapScene->addItem(copy);
    }

    // Remove last layer
    for(auto& item : m_copiedItemOnMap){
        TileItem *tile = m_mapScene->itemByIndex(indexByPos(item));
        tile->removeLastLayer();
    }

    m_mapScene->triggerTool(true, ToolType::Brush);
}

void AnchorCommand::redo()
{
    // Clear float selection
    for(const auto& item : *m_floatSelectionFromView){
        if(item->scene() != nullptr)
            m_mapScene->removeItem(item);
    }
    m_floatSelectionFromView->clear();

    // Anchor layer
    for(auto& item : m_copiedSelection){
        TileItem *tile = m_mapScene->itemByIndex(indexByPos(item));
        tile->addLayer(item->name(), item->brush(), true);
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
