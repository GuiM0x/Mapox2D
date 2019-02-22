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

bool TileItem::addLayer(const QString& textureName,
                        const QBrush& brush,
                        bool forceCompose)
{
    // Note : forceCompose is used by AnchorCommand::redo().
    //        It forces composing because when AnchorCommand::undo(),
    //        it simply removes the last layer. So we must have same
    //        'z-index' on layer when anchor.

    m_name = textureName;

    bool added{false};
    if(!m_layers.empty() &&
            (m_layers.top().name != textureName || forceCompose)){
        m_layers.push(Layer{textureName, brush});
        composeImage(brush);
        added = true;
    }
    if(m_layers.empty()){
        m_layers.push(Layer{textureName, brush});
        m_composed.push(brush);
        setBrush(brush);
        added = true;
    }
    return added;
}

void TileItem::removeLastLayer()
{
    if(!m_layers.empty()){
        m_layers.pop();
    }
    if(!m_composed.empty()){
        m_composed.pop();
        if(!m_composed.empty())
            setBrush(m_composed.top());
    }
    if(m_layers.empty()){
        clear();
    }
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
    assert(!m_layers.empty() && !m_composed.empty());

    QImage newImage    = brush.textureImage();
    QImage actualImage = m_composed.top().textureImage();

    if(!actualImage.isNull()){
        QPainter painter{&newImage};
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        painter.drawImage(0, 0, actualImage);
    }

    const QBrush newBrush{newImage};
    m_composed.push(newBrush);
    setBrush(newBrush);
}

bool TileItem::canAddLayer(const QString& textureName) const
{
    return m_layers.empty() || (m_layers.top().name != textureName);
}

void TileItem::clear()
{
    m_layers = std::stack<Layer>{};
    m_composed = std::stack<QBrush>{};
    m_name = "";
    setBrush(QBrush{QPixmap{}});
}

void TileItem::copyLayers(std::stack<Layer> layers)
{
    clear();
    std::stack<TileItem::Layer> copy{};
    while(!layers.empty()){
        copy.push(layers.top());
        layers.pop();
    }
    while(!copy.empty()){
        addLayer(copy.top().name,
                 copy.top().brush);
        copy.pop();
    }
}

QImage TileItem::image() const
{
    if(m_composed.empty())
        return QImage{};
    return m_composed.top().textureImage();
}

///// STATIC FUNCTION
TileItem* TileItem::copy(TileItem* itemToCopy)
{
    const QSizeF size = itemToCopy->rect().size();
    TileItem *copiedItem = new TileItem{QRectF{QPointF{0, 0}, size}};
    copiedItem->setPos(itemToCopy->scenePos());
    copiedItem->setPen(itemToCopy->pen());
    copiedItem->setName(itemToCopy->name());
    copiedItem->setIndex(itemToCopy->index());
    copiedItem->copyLayers(itemToCopy->layers());

    return copiedItem;
}
