#include "texturelist.h"

TextureList::TextureList(QWidget *parent)
    : QListWidget{parent}
{
    setIconSize(QSize{64, 64});
}

void TextureList::addTexture(const QString& fileName)
{
    const QString cuttedFileName = StringTools::cutFileName(fileName);
    const QString cuttedExtension = StringTools::cutExtensionFileName(cuttedFileName);

    if(!textureAlreadyExists(cuttedExtension)){
        m_texturesPaths[cuttedExtension] = fileName;
        QPixmap texture{fileName};
        if(texture.isNull())
            throw(std::runtime_error{"TextureList::addTexture() - texture null - " + fileName.toStdString()});
        m_textures[cuttedExtension] = texture;
        QPixmap scaledTexture = texture.scaled(iconSize());
        QIcon icon{scaledTexture};
        QListWidgetItem *item = new QListWidgetItem{icon, cuttedExtension};
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

void TextureList::clean()
{
    clear();
    m_textures.clear();
    m_texturesPaths.clear();
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
