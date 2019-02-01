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

void MapView::holdStatusBar(QStatusBar *statusBar)
{
    m_statusBar = statusBar;
}

void MapView::reset(const QRectF& mapSceneBounding)
{
    setSceneRect(mapSceneBounding);
    resetTransform();

    m_originalItemsSelected.clear();
    m_copiedItems.clear();
    m_pastedItems.clear();
    toolTriggered(true, ToolType::Brush);
}

void MapView::mouseMovingAndPressing()
{
    if(m_brushToolActived && !m_keysState["Shift"]){
        fillTile();
    }

    if(canDragPastedSelection()){
        adjustFocusRectPastedSelectionDragging();
        movePastedSelection();
    }
}

void MapView::rubberChanged(const QRect& rubberBandRect)
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

void MapView::toolTriggered(bool trigger, ToolType type)
{
    m_selectionToolActived = false;
    m_moveSelectionToolActived = false;
    m_brushToolActived = false;

    switch(type){
    case ToolType::Selection:
        m_selectionToolActived = trigger;
        if(m_selectionToolActived){
            QCursor cursor{Qt::SizeFDiagCursor};
            setCursor(cursor);
            emit checkTool(ToolType::Selection);
        } else {
            QCursor cursor{Qt::ArrowCursor};
            setCursor(cursor);
        }
        break;
    case ToolType::MoveSelection:
        m_moveSelectionToolActived = trigger;
        if(m_moveSelectionToolActived){
            QCursor cursor{Qt::SizeAllCursor};
            setCursor(cursor);
            emit checkTool(ToolType::MoveSelection);
        } else {
            QCursor cursor{Qt::ArrowCursor};
            setCursor(cursor);
        }
        break;
    case ToolType::Brush:
        m_brushToolActived = trigger;
        if(m_brushToolActived){
            QCursor cursor{QPixmap{":/icons/brushTileCursor"}};
            setCursor(cursor);
            emit checkTool(ToolType::Brush);
        } else {
            QCursor cursor{Qt::ArrowCursor};
            setCursor(cursor);
        }
        break;
    case ToolType::NoTool:
        emit checkTool(ToolType::NoTool);
        QCursor cursor{Qt::ArrowCursor};
        setCursor(cursor);
        break;
    }

    // IF MOVE TOOL DISABLED AND LAST COMMAND IS "PASTE SELECTION"
    // -> QUndoStack::undo()
    // Why ? Because it "remove" copied items and avoids some programming pain
    if(!m_moveSelectionToolActived && m_undoStack->count() > 0 && !m_pastedItems.empty()){
        auto lastCommand = m_undoStack->command(m_undoStack->count() - 1);
        assert(lastCommand != nullptr);
        if(lastCommand->actionText() == "paste selection"){
            m_undoStack->undo();
        }
    }

    // IF SELECTION TOOL DISABLED
    // -> remove selection
    if(!m_selectionToolActived && !m_originalItemsSelected.empty()){
        restoreOriginalSeletedItem();
    }
}

void MapView::copyTriggered()
{
    // Items Selected ?
    if(!m_originalItemsSelected.empty()){
        if(!m_copiedItems.empty()) m_copiedItems.clear();
        // Append to copied items List
        for(const auto& item : m_originalItemsSelected){
            m_copiedItems.append(copyTile(std::get<0>(item)));
        }
        // Sort by original index
        std::sort(std::begin(m_copiedItems), std::end(m_copiedItems),
                  [](TileItem *item1, TileItem *item2){
                        return item1->index() < item2->index();
                  });
        // Restore selected items
        restoreOriginalSeletedItem();
    }
}

void MapView::pasteTriggered()
{
    if(!m_copiedItems.empty()){
        auto mapScene = static_cast<MapScene*>(scene());
        QUndoCommand *pasteCommand = new PasteCommand{mapScene, m_copiedItems, &m_pastedItems};
        m_undoStack->push(pasteCommand);
        emit activeAnchorAct(true);
    }
}

void MapView::itemFocusChanged()
{
    if(canDragPastedSelection()){
        const QCursor cursor{Qt::SizeAllCursor};
        setCursor(cursor);
    }

    // WARNING : canAnchor() must be call after canDragPastedSelection()
    //           This is needed to avoid a bug when cursor get out of copied selection rect
    //           and doesn't change.
    if(canAnchor()){
        const QCursor cursor{QPixmap{":/icons/anchorCursor.png"}};
        setCursor(cursor);
    }
}

void MapView::fillSelection()
{
    auto *mapScene = static_cast<MapScene*>(scene());
    const QString textureSelectedInList = mapScene->currentTextureName();
    if(!textureSelectedInList.isEmpty()){
        QUndoCommand *fillSelectionCommand = new FillSelectionCommand{mapScene,
                                                                      textureSelectedInList,
                                                                      m_originalItemsSelected};
        m_undoStack->push(fillSelectionCommand);
    } else {
        if(m_statusBar != nullptr)
            m_statusBar->showMessage("Cannot fill tile - No texture selected in list !", 3000);
    }
}

void MapView::anchorSelection()
{
    if(canAnchor()){
        anchorSelection();
    }
}

void MapView::undoActTriggered(bool trigger)
{
    if(trigger){
        emit activeAnchorAct(canAnchor());
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
        if(canAnchor()){
            anchorPastedSelection();
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_keysState["LeftButton"] = false;
        if(canDragPastedSelection()){
            adjustPastedSelectionPosEndDrag();
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

QRectF MapView::pastedSelectionBoundingRect() const
{
    assert(!m_pastedItems.empty()
           && "MapView::pastedSelectionBoundingRect - m_pastedItems cannot be empty");
    const QPointF startPoint = m_pastedItems[0]->scenePos();
    const TileItem *lastItem = m_pastedItems.back();
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

bool MapView::canDragPastedSelection() const
{
    if(m_moveSelectionToolActived && !m_pastedItems.empty()){
        const auto mapScene = static_cast<MapScene*>(scene());
        const QRectF sceneRect = QRectF{QPointF{0, 0}, QSizeF{mapScene->sceneRect().size()}};
        const QPointF mousePos = mapScene->mousePosition();
        const QRectF pastedSelectionRect = pastedSelectionBoundingRect();
        if(sceneRect.contains(mousePos) && pastedSelectionRect.contains(mousePos)){
            return true;
        }
    }
    return false;
}

bool MapView::canAnchor()
{
    if(m_moveSelectionToolActived && !m_pastedItems.empty()){
        const auto mapScene = static_cast<MapScene*>(scene());
        const QPointF mousePos = mapScene->mousePosition();
        const QRectF copiedSelectionRect = pastedSelectionBoundingRect();
        if(mousePos.x() < copiedSelectionRect.x() ||
                mousePos.x() >= copiedSelectionRect.x() + copiedSelectionRect.width() ||
                mousePos.y() < copiedSelectionRect.y() ||
                mousePos.y() >= copiedSelectionRect.y() + copiedSelectionRect.height()){
            emit activeAnchorAct(true);
            return true;
        }
    }
    emit activeAnchorAct(false);
    return false;
}

void MapView::adjustFocusRectPastedSelectionDragging()
{
    assert(!m_pastedItems.empty() && "m_pastedItems cannot be empty");
    const auto mapScene = static_cast<MapScene*>(scene());
    const QPointF firstItemPos = m_pastedItems[0]->scenePos();
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

void MapView::adjustPastedSelectionPosEndDrag()
{
    assert(!m_pastedItems.empty() && "m_pastedItems cannot be empty");
    const auto mapScene = static_cast<MapScene*>(scene());
    const QPointF firstItemPos = m_pastedItems[0]->scenePos();
    const QPointF focusRectPos = mapScene->focusRect()->scenePos();
    const QPointF movement = focusRectPos - firstItemPos;
    for(auto&& item : m_pastedItems){
        item->moveBy(movement.x(), movement.y());
    }
}

void MapView::movePastedSelection()
{
    MapScene *mapScene = static_cast<MapScene*>(scene());

    // Movement Base
    QPointF movement = mapScene->mouseMoveVector();

    // If copied selection rect is out of map -> adjust movement
    const QRectF copiedSelectionRect = pastedSelectionBoundingRect();
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
    for(const auto& item : m_pastedItems){
        item->moveBy(movement.x(), movement.y());
    }
}

void MapView::anchorPastedSelection()
{
    auto mapScene = static_cast<MapScene*>(scene());
    QUndoCommand *anchorCommand = new AnchorCommand{mapScene, &m_pastedItems};
    m_undoStack->push(anchorCommand);
    emit activeAnchorAct(false);
}
