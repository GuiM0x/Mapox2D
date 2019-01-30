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
    int index() const;
    QString name() const;
    void setIndex(int index);
    void setName(const QString& name);

private:
    QString m_name{""};
    int m_index{-1};
};

#endif // TILEITEM_H
