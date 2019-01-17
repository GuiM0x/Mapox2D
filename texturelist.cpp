#include "texturelist.h"

TextureList::TextureList(QWidget *parent)
    : QListWidget{parent}
{
    setIconSize(QSize{64, 64});
}

void TextureList::addTexture(const QString& fileName)
{
    const QString cuttedFileName = StringTools::cutFileName(fileName);

    if(!textureAlreadyExists(cuttedFileName)){
        m_texturesPaths[cuttedFileName] = fileName;
        QPixmap texture{fileName};
        m_textures[cuttedFileName] = texture;
        QPixmap scaledTexture = texture.scaled(iconSize());
        QIcon icon{scaledTexture};
        QListWidgetItem *item = new QListWidgetItem{icon, cuttedFileName};
        addItem(item);
    }
}

QPixmap TextureList::getTexture(const QString &textureName)
{
    if(m_textures.find(textureName) != std::end(m_textures))
        return m_textures[textureName].copy();
    return QPixmap{};
}

std::map<QString, QPixmap>* TextureList::textureList()
{
    return &m_textures;
}

bool TextureList::textureAlreadyExists(const QString &fileName)
{
    auto it = m_textures.find(fileName);
    if(it != std::end(m_textures)){
        QMessageBox::information(this, "Mapox2D",
                               tr("Texture already loaded !"));
        return true;
    }
    return false;
}
