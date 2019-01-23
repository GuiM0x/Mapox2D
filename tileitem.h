#ifndef TILEITEM_H
#define TILEITEM_H

#include <QGraphicsRectItem>

class TileItem : public QGraphicsRectItem
{
public:
    TileItem(QGraphicsItem *parent = nullptr);
    TileItem(const QRectF &rect, QGraphicsItem *parent = nullptr);
    TileItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);

private:
    QString m_textureName{};
    int     m_index{};
};

#endif // TILEITEM_H
