#ifndef TILEITEM_H
#define TILEITEM_H

#include <stack>

#include <QDebug>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QRegularExpression>

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
    bool addLayer(const QString& textureName,
                  const QBrush& texture,
                  bool forceCompose = false);
    void removeLastLayer();
    int layerCount() const;
    std::stack<Layer> layers() const;
    bool canAddLayer(const QString& textureName) const;
    void clear();
    void copyLayers(std::stack<Layer> layers);

    static TileItem* copy(TileItem* item);

private:
    void composeImage(const QBrush& texture);
    void setBrush(const QBrush& brush);
    void updateComposedName();

private:
    QString                 m_name{""};
    int                     m_index{-1};
    std::stack<Layer>       m_layers{};
    std::stack<QBrush>      m_composed{};
};

#endif // TILEITEM_H
