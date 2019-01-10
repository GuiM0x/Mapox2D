#include "mapview.h"

MapView::MapView(QWidget *parent)
    : QGraphicsView{parent}
{
    setMouseTracking(true);
    createKeys();
}

void MapView::focusOutEvent(QFocusEvent *event)
{
    if(event->lostFocus()){
        m_keysState["Control"] = false;
    }
}

void MapView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        m_keysState["Control"] = true;
    }

    if(event->key() == Qt::Key_Shift){
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

void MapView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        m_keysState["Control"] = false;
    }

    if(event->key() == Qt::Key_Shift){
        setDragMode(QGraphicsView::NoDrag);
    }
}

void MapView::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    if(m_keysState["Control"]){
        m_zoomFactor = 1.0 + (numDegrees.y() / 300.0);
        scale(m_zoomFactor, m_zoomFactor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void MapView::createKeys()
{
    m_keysState["Control"] = false;
}
