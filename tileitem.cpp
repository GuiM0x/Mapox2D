#include "tileitem.h"

TileItem::TileItem(QGraphicsItem *parent)
    : QGraphicsRectItem{parent}
{

}

TileItem::TileItem(const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem{rect, parent}
{

}

TileItem::TileItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsRectItem{x, y, width, height, parent}
{

}
