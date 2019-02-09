#ifndef TILEITEM_H
#define TILEITEM_H

#include <stack>

#include <QDebug>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QPen>
#include <QBrush>

class TileItem : public QGraphicsRectItem
{
public:
    struct Layer{
        QString name{};
        QBrush  brush{};
    };

public:
    TileItem(QGraphicsItem *parent = nullptr);
    TileItem(const QRectF &rect, QGraphicsItem *parent = nullptr);
    TileItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);

public:
    int index() const;
    QString name() const;
    void setIndex(int index);
    void setName(const QString& name);

    bool addLayer(const QString& textureName, const QBrush& texture);
    void removeLastLayer();
    bool isComposed() const;
    int layerCount() const;
    std::stack<Layer> layers() const;

    static TileItem* copy(TileItem* item);

//private:
    void setBrush(const QBrush& brush);

private:
    void composeImage(const QBrush& texture);

private:
    QString                 m_name{""};
    int                     m_index{-1};
    std::stack<Layer>       m_layers{};
};

#endif // TILEITEM_H
