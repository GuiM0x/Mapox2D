#include "mapscene.h"

MapScene::MapScene(QObject *parent)
    : QGraphicsScene{parent}
{

}

void MapScene::holdStatusBar(QStatusBar *statusBar)
{
    m_statusBar = statusBar;
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_mousePosStr = "x(" + QString::number(mouseEvent->scenePos().x(), 'f', 0) +
                    "), y(" +
                    QString::number(mouseEvent->scenePos().y(), 'f', 0) + ")";

    if(m_statusBar != nullptr)
        m_statusBar->showMessage(m_mousePosStr);
}
