#include "texturelist.h"

TextureList::TextureList(QWidget *parent)
    : QListWidget{parent}
{
    setMouseTracking(true);
    setIconSize(QSize{64, 64});
    setResizeMode(QListView::Adjust);
    setViewMode(QListView::IconMode);
}

void TextureList::addTexture(const QString& fileName, bool fromLoadFile)
{
    const QString cuttedFileName = StringTools::cutFileName(fileName);
    const QString textureName = StringTools::cutExtensionFileName(cuttedFileName);

    if(!textureAlreadyExists(textureName)){
        QPixmap texture{fileName};
        if(fileName.isEmpty())
            throw(std::runtime_error{"TextureList::addTexture() - filename empty - "});
        if(texture.isNull())
            throw(std::runtime_error{"TextureList::addTexture() - texture null - " + fileName.toStdString()});
        m_textures[textureName] = texture;
        QPixmap scaledTexture = texture.scaled(iconSize());
        QIcon icon{scaledTexture};
        QListWidgetItem *item = new QListWidgetItem{icon, ""};
        item->setSizeHint(QSize{iconSize() + QSize{4, 4}});
        item->setToolTip(textureName);
        addItem(item);
        if(!fromLoadFile)
            emit docModified();
    }
}

void TextureList::addTexture(const QList<QPixmap>& textures, const QString& fileName)
{
    const QString cuttedFileName = StringTools::cutFileName(fileName);
    const QString cuttedExtension = StringTools::cutExtensionFileName(cuttedFileName);

    bool good{true};

    for(int i = 0; i < textures.size(); ++i){
        const QString textureName = cuttedExtension + "(" + QString::number(i) + ")";
        if(!textureAlreadyExists(textureName)){
            const QPixmap texture = textures[i];
            m_textures[textureName] = texture;
            QPixmap scaledTexture = texture.scaled(iconSize());
            QIcon icon{scaledTexture};
            QListWidgetItem *item = new QListWidgetItem{icon, ""};
            item->setSizeHint(QSize{iconSize() + QSize{4, 4}});
            item->setToolTip(textureName);
            addItem(item);
        } else {
            good = false;
            break;
        }
    }

    if(good)
        emit docModified();
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
