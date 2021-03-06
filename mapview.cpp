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
    m_floatSelection.clear();
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

    // IF MOVE TOOL DISABLED AND FLOAT SELECTION NOT EMPTY
    // Command -> Discard Float selection (e.g pasted items)

    // TO DO : Find another condition(s) or else discard float selection make dick move
    //         The fact is that discard's command intersects the float selection between other commands
    //         Maybe an action in tool bar for beginning ?
    if(!m_moveSelectionToolActived && !m_floatSelection.empty()){
        /*auto mapScene = static_cast<MapScene*>(scene());
        DiscardFloatSelectionCommand *dfsc = new DiscardFloatSelectionCommand{mapScene,
                                                                              &m_floatSelection};
        m_undoStack->push(dfsc);*/

       // m_undoStack->undo(); <- don't do that ! Never !
    }

    // IF SELECTION TOOL DISABLED
    // -> remove selection
    if(!m_selectionToolActived && !m_originalItemsSelected.empty()){
        restoreOriginalSeletedItem();
    }
}

void MapView::copyTriggered()
{
    if(!m_originalItemsSelected.empty()){
        copyItemsSelected();
        restoreOriginalSeletedItem();
    }
}

void MapView::cutTriggered()
{
    if(!m_originalItemsSelected.empty()){
        copyItemsSelected();
        auto mapScene = static_cast<MapScene*>(scene());
        QUndoCommand *deleteSelectionCommand = new DeleteSelectionCommand{mapScene,
                                                                          &m_originalItemsSelected};
        m_undoStack->push(deleteSelectionCommand);
        deleteSelectionCommand->setText("Selection cutted\ncut selection");
        restoreOriginalSeletedItem();
    }
}

void MapView::pasteTriggered()
{
    if(!m_copiedItems.empty() && m_floatSelection.empty()){
        auto mapScene = static_cast<MapScene*>(scene());
        QUndoCommand *pasteCommand = new PasteCommand{mapScene, m_copiedItems, &m_floatSelection};
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
        anchorPastedSelection();
    }
}

void MapView::undoActTriggered(bool trigger)
{
    if(trigger){
        emit activeAnchorAct(canAnchor());
    }
}

void MapView::selectAll()
{
    restoreOriginalSeletedItem();
    QList<QGraphicsItem*> allItems = items();
    for(const auto& item : allItems){
        TileItem *itemSelected = qgraphicsitem_cast<TileItem*>(item);
        // Avoid selection of focusRect - it's the only one with no pen
        if(itemSelected->pen().style() != Qt::NoPen){
            m_originalItemsSelected.push_back(std::make_tuple(itemSelected, itemSelected->pen()));
            itemSelected->setOpacity(0.5);
            QPen pen{Qt::DashLine};
            pen.setBrush(QBrush{Qt::white});
            itemSelected->setPen(pen);
        }
    }
}

void MapView::removeTileFromScene(const QString& tileName)
{
    // Slot connected with signal Texture::removeTileFromScene

    assert(!tileName.isEmpty());
    MapScene *mapScene = static_cast<MapScene*>(scene());
    const std::vector<TileItem*> *tiles = mapScene->tiles();
    restoreOriginalSeletedItem();
    for(const auto& item : *tiles){
        const std::string itemName = item->name().toStdString();
        if(!itemName.empty()){
            if(itemName.find(tileName.toStdString()) != std::string::npos){
                // Now we push item in m_originalSelected.
                m_originalItemsSelected.push_back(std::make_tuple(item, item->pen()));
                item->setOpacity(0.5);
                QPen pen{Qt::DashLine};
                pen.setBrush(QBrush{Qt::white});
                item->setPen(pen);
            }
        }
    }
    // And here we go : delete selection command
    DeleteSelectionCommand *deleteSelection =
            new DeleteSelectionCommand{mapScene,
                                       &m_originalItemsSelected};
    m_undoStack->push(deleteSelection);
    restoreOriginalSeletedItem();
}

void MapView::resizeGridTriggered()
{
    using Rows = int;
    using Cols = int;
    MapScene *mapScene = static_cast<MapScene*>(scene());
    ResizeGridDialog dial{mapScene->rows(), mapScene->cols(), this};
    std::pair<Rows, Cols> result = dial.processDial();
    const int rows = std::get<0>(result);
    const int cols = std::get<1>(result);
    if(rows != 0 || cols != 0){
        mapScene->resizeGrid(rows, cols);
        m_undoStack->clear();
        reset(mapScene->itemsBoundingRect());
        // emit documentModified or Command resize ?
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
        if(!m_originalItemsSelected.empty()){
            QUndoCommand *deleteSelectionCommand = new DeleteSelectionCommand{mapScene,
                                                                              &m_originalItemsSelected};
            m_undoStack->push(deleteSelectionCommand);
            restoreOriginalSeletedItem();
        } else {
            int index = mapScene->currentTile();
            if(mapScene->canDeleteTile(index)){
                QUndoCommand *deleteCommand = new DeleteTileCommand{mapScene};
                m_undoStack->push(deleteCommand);
            }
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
                QPen pen{Qt::DashLine};
                pen.setBrush(QBrush{Qt::white});
                itemSelected->setPen(pen);
            }
        }
    }
}

QRectF MapView::pastedSelectionBoundingRect() const
{
    assert(!m_floatSelection.empty()
           && "MapView::pastedSelectionBoundingRect - m_floatSelection cannot be empty");
    const QPointF startPoint = m_floatSelection[0]->scenePos();
    const TileItem *lastItem = m_floatSelection.back();
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
    if(m_moveSelectionToolActived && !m_floatSelection.empty()){
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
    if(m_moveSelectionToolActived && !m_floatSelection.empty()){
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
    assert(!m_floatSelection.empty() && "m_floatSelection cannot be empty");
    const auto mapScene = static_cast<MapScene*>(scene());
    const QPointF firstItemPos = m_floatSelection[0]->scenePos();
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
    assert(!m_floatSelection.empty() && "m_floatSelection cannot be empty");
    const auto mapScene = static_cast<MapScene*>(scene());
    const QPointF firstItemPos = m_floatSelection[0]->scenePos();
    const QPointF focusRectPos = mapScene->focusRect()->scenePos();
    const QPointF movement = focusRectPos - firstItemPos;
    for(auto&& item : m_floatSelection){
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
    for(const auto& item : m_floatSelection){
        item->moveBy(movement.x(), movement.y());
    }
}

void MapView::anchorPastedSelection()
{
    auto mapScene = static_cast<MapScene*>(scene());
    QUndoCommand *anchorCommand = new AnchorCommand{mapScene, &m_floatSelection};
    m_undoStack->push(anchorCommand);
    emit activeAnchorAct(false);
}

void MapView::copyItemsSelected()
{
    if(!m_copiedItems.empty()) m_copiedItems.clear();

    for(const auto& item : m_originalItemsSelected){
        m_copiedItems.append(TileItem::copy(std::get<0>(item)));
    }

    std::sort(std::begin(m_copiedItems), std::end(m_copiedItems),
              [](TileItem *item1, TileItem *item2){
                    return item1->index() < item2->index();
              });
}
