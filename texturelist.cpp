#include "texturelist.h"

TextureList::TextureList(QWidget *parent)
    : QListWidget{parent}
{
    setIconSize(QSize{64, 64});
}

void TextureList::addTexture(const QString& fileName)
{
    m_texturesPaths[cutFileName(fileName)] = fileName;
    QPixmap texture{fileName};
    m_textures[cutFileName(fileName)] = texture;
    QPixmap scaledTexture = texture.scaled(iconSize());
    QIcon icon{scaledTexture};
    QListWidgetItem *item = new QListWidgetItem{icon, cutFileName(fileName)};
    addItem(item);
}

QPixmap TextureList::getTexture(const QString &textureName)
{
    if(m_textures.find(textureName) != std::end(m_textures))
        return m_textures[textureName].copy();
    return QPixmap{};
}

QString TextureList::cutFileName(const QString& fileName) const
{
    QString cutted{};
    int index{};
    for(int i = fileName.length(); i > 0; --i){
        if(fileName[i-1] == '/'){
            index = i;
            break;
        }
    }
    for(int i = index; i < fileName.size(); ++i){
        cutted += fileName[i];
    }
    return cutted;
}
