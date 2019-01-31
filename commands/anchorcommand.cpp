#include "anchorcommand.h"

AnchorCommand::AnchorCommand(MapScene *mapScene,
                             QList<TileItem*> *pastedTiles,
                             QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_pastedTiles = pastedTiles;
    assert(m_mapScene != nullptr && m_pastedTiles != nullptr);
    QString infos = "Anchor Selection\nanchor selection";
    setText(infos);
    for(const auto& item : *m_pastedTiles){
        const QPointF pos = item->scenePos();
        m_firstAnchorPos.push_back(std::make_tuple(item, pos));
    }
}

void AnchorCommand::undo()
{
    qDebug() << "AnchorCommand::undo()";
    m_pastedTiles->clear();
    for(const auto& item : m_removedPastedTiles){
        auto removedPastedItem = std::get<0>(item);
        m_pastedTiles->push_back(removedPastedItem);
        m_mapScene->addItem(removedPastedItem);
        m_mapScene->fillTile(indexByPos(removedPastedItem), std::get<1>(item));
    }
    m_removedPastedTiles.clear();
}

void AnchorCommand::redo()
{
    qDebug() << "AnchorCommand::redo()";
    m_removedPastedTiles.clear();
    for(const auto& it : m_firstAnchorPos){
        auto item = std::get<0>(it);
        const QPointF pos = std::get<1>(it);
        item->setPos(pos);
    }
    for(const auto& item : *m_pastedTiles){
        const int index = indexByPos(item);
        auto oldTile = m_mapScene->itemByIndex(index);
        qDebug() << "Old Tile : " << oldTile->name();
        m_removedPastedTiles.push_back(std::make_tuple((item), oldTile->name()));
        // INFO : removeItem() doesn't destroy ptr
        //        it just removes ownership by Scene and pass it to the caller
        m_mapScene->removeItem(item);
        m_mapScene->fillTile(index, item->name());
    }
    m_pastedTiles->clear();
    m_mapScene->triggerTool(true, ToolType::MoveSelection);
}

int AnchorCommand::indexByPos(TileItem *item) const
{
    const QPointF pos = item->scenePos();
    const int index_x = static_cast<int>(pos.x()) / m_mapScene->tileWidth();
    const int index_y = static_cast<int>(pos.y()) / m_mapScene->tileHeight();
    const int index = (index_y * m_mapScene->cols()) + index_x;
    return index;
}
