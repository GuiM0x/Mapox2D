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

int TileItem::index() const
{
    return m_index;
}

QString TileItem::name() const
{
    return m_name;
}

void TileItem::setIndex(int index)
{
    m_index = index;
}

void TileItem::setName(const QString& name)
{
    m_name = name;
}
