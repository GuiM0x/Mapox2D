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
    const qreal width  = m_mapScene->tileWidth();
    const qreal height = m_mapScene->tileHeight();
    for(const auto& item : *m_floatSelectionFromView){
        TileItem *tile = UtilityTools::copyTile(item, QSizeF{width, height});
        m_floatSelectionSaved.push_back(tile);
        // Save first anchor pos
        m_firstAnchorPos.push_back(std::make_tuple(tile, item->scenePos()));
    }

    QString infos = "Anchor Selection\nanchor selection";
    setText(infos);
}

void AnchorCommand::undo()
{
    for(const auto& it : m_firstAnchorPos){
        const auto item = std::get<0>(it);
        const QPointF pos = std::get<1>(it);
        item->setPos(pos);
    }

    m_floatSelectionFromView->clear();
    for(const auto& item : m_floatSelectionSaved){
        m_floatSelectionFromView->push_back(item);
        m_mapScene->addItem(item);
    }

    for(const auto& oldItem : m_oldItemOnMap){
        const int index = std::get<0>(oldItem);
        const QString textureName = std::get<1>(oldItem);
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
    for(const auto& item : m_floatSelectionSaved){
        const int index = indexByPos(item);
        const auto oldItem = m_mapScene->itemByIndex(index);
        m_oldItemOnMap.push_back(std::make_tuple(index, oldItem->name()));
        m_mapScene->fillTile(index, item->name());
    }
}

int AnchorCommand::indexByPos(TileItem *item) const
{
    const QPointF pos = item->scenePos();
    const int index_x = static_cast<int>(pos.x()) / m_mapScene->tileWidth();
    const int index_y = static_cast<int>(pos.y()) / m_mapScene->tileHeight();
    const int index = (index_y * m_mapScene->cols()) + index_x;
    return index;
}
