#ifndef TILEITEM_H
#define TILEITEM_H

#include <QGraphicsRectItem>

class TileItem : public QGraphicsRectItem
{
public:
    TileItem(QGraphicsItem *parent = nullptr);
    TileItem(const QRectF &rect, QGraphicsItem *parent = nullptr);
    TileItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);

public:
    QString name() const;
    void setName(const QString& name);

private:
    QString m_name{};
};

#endif // TILEITEM_H
