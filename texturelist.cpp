#include "texturelist.h"

TextureList::TextureList(QWidget *parent)
    : QListWidget{parent}
{
    setIconSize(QSize{64, 64});
    connect(this, &QListWidget::itemClicked, this, &TextureList::currentActiveItem);
}

void TextureList::currentActiveItem(QListWidgetItem *item)
{
    qDebug() << item->text();
}

void TextureList::addTexture(const QString& fileName)
{
    QPixmap texture{fileName};
    QPixmap scaledTexture = texture.scaled(iconSize());
    QIcon icon{scaledTexture};
    QListWidgetItem *item = new QListWidgetItem{icon, fileName};
    addItem(item);
}
