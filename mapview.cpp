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
}

void MapView::mouseMovingAndPressing()
{
    if(!m_selectionToolActived && !m_keysState["Shift"]){
        fillTile();
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
}

void MapView::copyTriggered()
{
    qDebug() << "MapView::copyTriggered()";

    // Items Selected ?
    if(!m_originalItemsSelected.empty()){
        if(!m_tmpCopiedItem.empty()) m_tmpCopiedItem.clear();
        // Append to copied items List
        for(const auto& item : m_originalItemsSelected){
            m_tmpCopiedItem.append(copyTile(std::get<0>(item)));
        }
    }
}

void MapView::pasteTriggered()
{
    qDebug() << "MapView::pasteTriggered()";

    if(!m_tmpCopiedItem.empty()){
        const auto mapScene = static_cast<MapScene*>(scene());
        // TO DO : Sort function ? Index in TileItem ?
        std::sort(std::begin(m_tmpCopiedItem), std::end(m_tmpCopiedItem),
                  [mapScene](TileItem *item1, TileItem *item2){
                        const QPointF pos1 = item1->scenePos();
                        const QSize size1{mapScene->tileWidth(), mapScene->tileHeight()};
                        int index1 = UtilityTools::indexRelativeToPos(pos1,
                                                                     size1,
                                                                     mapScene->rows(),
                                                                     mapScene->cols());
                        const QPointF pos2 = item2->scenePos();
                        const QSize size2{mapScene->tileWidth(), mapScene->tileHeight()};
                        int index2 = UtilityTools::indexRelativeToPos(pos2,
                                                                     size2,
                                                                     mapScene->rows(),
                                                                     mapScene->cols());
                        return index1 < index2;
                  });
        // Add Items to Scene
        for(const auto& tileCopied : m_tmpCopiedItem){
            // Only for DEBUG
            const QPointF pos = tileCopied->scenePos();
            const QSize size{mapScene->tileWidth(), mapScene->tileHeight()};
            int index = UtilityTools::indexRelativeToPos(pos,
                                                         size,
                                                         mapScene->rows(),
                                                         mapScene->cols());
            // --> Tile Copied
            qDebug() << "Index [" << index << "] | "
                     << " | pos(" << pos.x() << ", " << pos.y() <<
                     ") | size(" << size.width() << ", " << size.height() <<
                        ") | name (" << tileCopied->name() << ")";

            mapScene->addItem(tileCopied);
            tileCopied->setBrush(QBrush{QColor{150, 255, 50}}); //////// Only for testing

            // TO DO in pasteTriggered() :
            //     - Float selection layer
            //     - Active (FLOAT SELECTION LAYER MOD)
            //
            // THEN :
            //     - Drag layer (snap to grid)
            //     - Anchor layer -> action (pasteTileCommand{});
            //
            // THEN IF ANCHOR :
            //     - FillTile with copied items
            //     - Remove items copied in scene
        }
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
        if(m_selectionToolActived)
            setDragMode(QGraphicsView::RubberBandDrag);
        if(!m_selectionToolActived && !m_keysState["Shift"]){
            setDragMode(QGraphicsView::NoDrag);
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
    TileItem *copiedItem = new TileItem{QRectF{QPointF{0, 0}, QSizeF{itemToCopy->boundingRect().size()}}};
    copiedItem->setPos(itemToCopy->scenePos());
    copiedItem->setPen(itemToCopy->pen());
    copiedItem->setBrush(itemToCopy->brush());
    copiedItem->setName(itemToCopy->name());

    /*qDebug() << "MapView::copyTile - Tile infos :\n"
             << "[" << copiedItem->name() << "] "
             << "(" << copiedItem->scenePos().x() << ", " << copiedItem->scenePos().y() <<")";*/

    return copiedItem;
}

