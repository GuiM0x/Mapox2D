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

bool TileItem::addLayer(const QString& textureName, const QBrush& brush)
{
    bool added{false};
    if(!m_layers.empty() && m_layers.top().name != textureName){
        m_layers.push(Layer{textureName, brush});
        composeImage(brush);
        added = true;
    }
    if(m_layers.empty()){
        m_layers.push(Layer{textureName, brush});
        setBrush(brush);
        added = true;
    }
    return added;
}

void TileItem::removeLastLayer()
{
    if(!m_layers.empty())
        m_layers.pop();
}

bool TileItem::isComposed() const
{
    return m_layers.size() > 1;
}

int TileItem::layerCount() const
{
    return static_cast<int>(m_layers.size());
}

std::stack<TileItem::Layer> TileItem::layers() const
{
    return m_layers;
}

void TileItem::setBrush(const QBrush& brush)
{
    QAbstractGraphicsShapeItem::setBrush(brush);
}

void TileItem::composeImage(const QBrush &brush)
{
    assert(!m_layers.empty());

    QImage newImage    = brush.textureImage();
    QImage actualImage = m_layers.top().brush.textureImage();

    QPainter painter{&newImage};
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.drawImage(0, 0, actualImage);

    setBrush(QBrush{newImage});
}

TileItem* TileItem::copy(TileItem* itemToCopy)
{
    const QSizeF size = itemToCopy->rect().size();
    TileItem *copiedItem = new TileItem{QRectF{QPointF{0, 0}, size}};
    copiedItem->setPos(itemToCopy->scenePos());
    copiedItem->setPen(itemToCopy->pen());
    copiedItem->setName(itemToCopy->name());
    copiedItem->setIndex(itemToCopy->index());

    copiedItem->setBrush(itemToCopy->brush());

    /*std::stack<TileItem::Layer> layers = itemToCopy->layers();
    std::stack<TileItem::Layer> copy{};
    while(!layers.empty()){
        copy.push(layers.top());
        layers.pop();
    }
    while(!copy.empty()){
        copiedItem->addLayer(copy.top().name,
                             copy.top().brush);
        copy.pop();
    }*/

    return copiedItem;
}
