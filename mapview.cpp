#include "mapview.h"

MapView::MapView(QWidget *parent)
    : QGraphicsView{parent}
{
    setMouseTracking(true);
    createKeys();
    connect(this, &QGraphicsView::rubberBandChanged, this, &MapView::rubberChanged);
}

void MapView::holdUndoStack(QUndoStack *undoStack)
{
    m_undoStack = undoStack;
}

void MapView::reset(const QRectF& mapSceneBounding)
{
    setSceneRect(mapSceneBounding);
    resetTransform();
    m_originalItemsSelected.clear();
    m_tmpCopiedItem.clear();
}

void MapView::mouseMovingAndPressing()
{
    if(m_brushToolActived && !m_keysState["Shift"]){
        fillTile();
    }

    if(canDragCopiedSelection()){
        adjustFocusRectCopiedSelectionDragging();
        moveCopiedSelection();
    }
}

void MapView::rubberChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint)
{
    QList<QGraphicsItem*> itemsSelected = items(rubberBandRect);
    const std::size_t selectedSize = static_cast<std::size_t>(itemsSelected.size());
    const std::size_t oldSelectedSize = m_originalItemsSelected.size();

    if(selectedSize > oldSelectedSize){
        growToSelection(itemsSelected);
    }

    if(selectedSize < oldSelectedSize){
        shrinkToSelection(itemsSelected);
    }
}

void MapView::selectToolActived(bool actived)
{
    m_selectionToolActived = actived;
    if(m_selectionToolActived){
        m_moveSelectionToolActived = false;
        m_brushToolActived = false;
        QCursor cursor{Qt::SizeFDiagCursor};
        setCursor(cursor);
        emit uncheckMoveSelectionTool();
        emit uncheckBrushTileTool();
        emit checkSelectTool();
    } else {
        QCursor cursor{Qt::ArrowCursor};
        setCursor(cursor);
    }
}

void MapView::moveSelectionToolActived(bool actived)
{
    m_moveSelectionToolActived = actived;
    if(m_moveSelectionToolActived){
        m_selectionToolActived = false;
        m_brushToolActived = false;
        QCursor cursor{Qt::SizeAllCursor};
        setCursor(cursor);
        emit uncheckSelectTool();
        emit uncheckBrushTileTool();
        emit checkMoveSelectionTool();
    } else {
        QCursor cursor{Qt::ArrowCursor};
        setCursor(cursor);
    }
}

void MapView::brushTileToolActived(bool actived)
{
    m_brushToolActived = actived;
    if(m_brushToolActived){
        m_selectionToolActived = false;
        m_moveSelectionToolActived = false;
        QCursor cursor{QPixmap{":/icons/brushTileCursor"}};
        setCursor(cursor);
        emit uncheckSelectTool();
        emit uncheckMoveSelectionTool();
        emit checkBrushTileTool();
    } else {
        QCursor cursor{Qt::ArrowCursor};
        setCursor(cursor);
    }
}

void MapView::copyTriggered()
{
    // Items Selected ?
    if(!m_originalItemsSelected.empty()){
        if(!m_tmpCopiedItem.empty()) m_tmpCopiedItem.clear();
        // Append to copied items List
        for(const auto& item : m_originalItemsSelected){
            m_tmpCopiedItem.append(copyTile(std::get<0>(item)));
        }
        // Sort by original index
        std::sort(std::begin(m_tmpCopiedItem), std::end(m_tmpCopiedItem),
                  [](TileItem *item1, TileItem *item2){
                        return item1->index() < item2->index();
                  });
        // Restore selected items
        restoreOriginalSeletedItem();
    }
}

void MapView::pasteTriggered()
{
    // TO DO : AVOID DOUBLE CTRL + V ?
    if(!m_tmpCopiedItem.empty()){
        moveSelectionToolActived(true);
        auto mapScene = static_cast<MapScene*>(scene());
        QUndoCommand *pasteCommand = new PasteCommand{mapScene, &m_tmpCopiedItem};
        m_undoStack->push(pasteCommand);
    }
}

void MapView::focusOutEvent(QFocusEvent *event)
{
    if(event->lostFocus()){
        m_keysState["Shift"] = false;
        m_keysState["Control"] = false;
        setDragMode(QGraphicsView::NoDrag);
        restoreOriginalSeletedItem();
    }
}

void MapView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift){
        m_keysState["Shift"] = true;
        setDragMode(QGraphicsView::ScrollHandDrag);
    }

    if(event->key() == Qt::Key_Control){
        m_keysState["Control"] = true;
    }

    // DELETE A TILE
    if(event->key() == Qt::Key_Delete){
        auto mapScene = static_cast<MapScene*>(scene());
        int index = mapScene->currentTile();
        if(mapScene->canDeleteTile(index)){
            QUndoCommand *deleteCommand = new DeleteTileCommand{mapScene};
            m_undoStack->push(deleteCommand);
        }
    }
}

void MapView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift){
        m_keysState["Shift"] = false;
        if(!m_keysState["LeftButton"]){
            setDragMode(QGraphicsView::NoDrag);
        }
    }
    if(event->key() == Qt::Key_Control){
        m_keysState["Control"] = false;
    }
}

void MapView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    if(m_keysState["Control"]){
        m_zoomFactor = 1.0 + (numDegrees.y() / 300.0);
        scale(m_zoomFactor, m_zoomFactor);
    } else {
        QGraphicsView::wheelEvent(event); // forward to scene
    }
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_keysState["LeftButton"] = true;
        if(!m_keysState["Shift"]){
            if(m_selectionToolActived)
                setDragMode(QGraphicsView::RubberBandDrag);
            if(!m_selectionToolActived)
                setDragMode(QGraphicsView::NoDrag);
            if(m_brushToolActived)
                fillTile();
        }
        restoreOriginalSeletedItem(); 
    }

    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_keysState["LeftButton"] = false;
        if(canDragCopiedSelection()){
            adjustCopiedSelectionPosEndDrag();
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapView::createKeys()
{
    m_keysState["Control"]    = false;
    m_keysState["LeftButton"] = false;
    m_keysState["Shift"]      = false;
}

void MapView::restoreOriginalSeletedItem()
{
    for(const auto& item : m_originalItemsSelected){
        std::get<0>(item)->setOpacity(1);
        std::get<0>(item)->setPen(std::get<1>(item));
    }
    m_originalItemsSelected.clear();
}

void MapView::fillTile()
{
    auto mapScene = static_cast<MapScene*>(scene());
    int index = mapScene->currentTile();
    if(mapScene->canFillTile(index)){
        QUndoCommand *fillTileCommand = new FillTileCommand{mapScene};
        m_undoStack->push(fillTileCommand);
    }
}

void MapView::shrinkToSelection(const QList<QGraphicsItem*>& itemsSelected)
{
    const std::size_t selectedSize = static_cast<std::size_t>(itemsSelected.size());
    const std::size_t oldSelectedSize = m_originalItemsSelected.size();

    const std::size_t nbItemToDeselect = oldSelectedSize - selectedSize;
    for(std::size_t i = oldSelectedSize-1; i > i - nbItemToDeselect; --i){
        std::get<0>(m_originalItemsSelected[i])->setOpacity(1);
        QPen pen = std::get<1>(m_originalItemsSelected[i]);
        std::get<0>(m_originalItemsSelected[i])->setPen(pen);
        m_originalItemsSelected.pop_back();
    }
}

void MapView::growToSelection(const QList<QGraphicsItem*>& itemsSelected)
{
    for(const auto& item : itemsSelected){
        auto it = std::find_if(std::begin(m_originalItemsSelected), std::end(m_originalItemsSelected),
                               [item](const std::tuple<TileItem*, QPen>& itemOriginal){
                                   return std::get<0>(itemOriginal) == item;
                               });
        if(it == std::end(m_originalItemsSelected)){
            TileItem *itemSelected = qgraphicsitem_cast<TileItem*>(item);
            // Avoid selection of focusRect - it's the only one with no pen
            if(itemSelected->pen().style() != Qt::NoPen){
                m_originalItemsSelected.push_back(std::make_tuple(itemSelected, itemSelected->pen()));
                itemSelected->setOpacity(0.5);
                itemSelected->setPen(QPen{Qt::DotLine});
            }
        }
    }
}

TileItem* MapView::copyTile(TileItem *itemToCopy)
{
    const MapScene *mapScene = static_cast<MapScene*>(scene());
    const QSizeF sizeTile{static_cast<qreal>(mapScene->tileWidth()),
                          static_cast<qreal>(mapScene->tileHeight())};
    TileItem *copiedItem = new TileItem{QRectF{QPointF{0, 0}, sizeTile}};
    copiedItem->setPos(itemToCopy->scenePos());
    copiedItem->setPen(itemToCopy->pen());
    copiedItem->setBrush(itemToCopy->brush());
    copiedItem->setName(itemToCopy->name());
    copiedItem->setIndex(itemToCopy->index());
    return copiedItem;
}

QRectF MapView::copiedSelectionBoundingRect() const
{
    assert(!m_tmpCopiedItem.empty()
           && "MapView::copiedSelectionBoundingRect - m_tmpCopiedItem cannot be empty");
    const QPointF startPoint{m_tmpCopiedItem[0]->scenePos()};
    auto *lastItem = m_tmpCopiedItem.back();
    const QPointF lastItemPos = lastItem->scenePos();
    MapScene *mapScene = static_cast<MapScene*>(scene());
    const QSizeF itemSize{static_cast<qreal>(mapScene->tileWidth()),
                          static_cast<qreal>(mapScene->tileHeight())};
    const QPointF endPoint{lastItemPos.x() + itemSize.width(),
                          lastItemPos.y() + itemSize.height()};
    const QRectF boundingSelection{startPoint, QSizeF{endPoint.x() - startPoint.x(),
                                                      endPoint.y()- startPoint.y()}};
    return boundingSelection;
}

bool MapView::canDragCopiedSelection() const
{
    if(m_moveSelectionToolActived && !m_tmpCopiedItem.empty()){
        const auto mapScene = static_cast<MapScene*>(scene());
        const QRectF sceneRect = QRectF{QPointF{0, 0}, QSizeF{mapScene->sceneRect().size()}};
        const QPointF mousePos = mapScene->mousePosition();
        const QRectF copiedSelectionRect = copiedSelectionBoundingRect();
        if(sceneRect.contains(mousePos) && copiedSelectionRect.contains(mousePos)){
            return true;
        }
    }
    return false;
}

void MapView::adjustFocusRectCopiedSelectionDragging()
{
    assert(!m_tmpCopiedItem.empty() && "m_tmpCopiedItem cannot be empty");
    const auto mapScene = static_cast<MapScene*>(scene());
    const QPointF firstItemPos = m_tmpCopiedItem[0]->scenePos();
    // Based to the top left point of first item in copied Selection
    const int focusRectIndex = UtilityTools::indexRelativeToPos(firstItemPos,
                                                                QSize{mapScene->tileWidth(), mapScene->tileHeight()},
                                                                mapScene->rows(),
                                                                mapScene->cols());
    if(focusRectIndex != -1){
        const QPointF focusRectPos = mapScene->itemByIndex(focusRectIndex)->scenePos();
        mapScene->focusRect()->setPos(focusRectPos);
    }
}

void MapView::adjustCopiedSelectionPosEndDrag()
{
    assert(!m_tmpCopiedItem.empty() && "m_tmpCopiedItem cannot be empty");
    const auto mapScene = static_cast<MapScene*>(scene());
    const QPointF firstItemPos = m_tmpCopiedItem[0]->scenePos();
    const QPointF focusRectPos = mapScene->focusRect()->scenePos();
    const QPointF movement = focusRectPos - firstItemPos;
    for(auto&& item : m_tmpCopiedItem){
        item->moveBy(movement.x(), movement.y());
    }
}

void MapView::moveCopiedSelection()
{
    MapScene *mapScene = static_cast<MapScene*>(scene());

    // Movement Base
    QPointF movement = mapScene->mouseMoveVector();

    // If copied selection rect is out of map -> adjust movement
    const QRectF copiedSelectionRect = copiedSelectionBoundingRect();
    if(copiedSelectionRect.x() < 0)
        movement.rx() = copiedSelectionRect.x() * -1;
    if(copiedSelectionRect.y() < 0)
        movement.ry() = copiedSelectionRect.y() * -1;
    const QSize mapSize = QSize{mapScene->cols() * mapScene->tileWidth(),
                                mapScene->rows() * mapScene->tileHeight()};
    if((copiedSelectionRect.x() + copiedSelectionRect.width()) > mapSize.width()){
        movement.rx() = mapSize.width() - (copiedSelectionRect.x() + copiedSelectionRect.width());
    }
    if((copiedSelectionRect.y() + copiedSelectionRect.height()) > mapSize.height()){
        movement.ry() = mapSize.height() - (copiedSelectionRect.y() + copiedSelectionRect.height());
    }

    // Move copied selection
    for(const auto& item : m_tmpCopiedItem){
        item->moveBy(movement.x(), movement.y());
    }
}

/*void MapView::removeCopiedItem()
{
    for(const auto& item : m_tmpCopiedItem){
        scene()->removeItem(item);
    }
    m_tmpCopiedItem.clear();
}*/
