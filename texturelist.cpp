#include "texturelist.h"

TextureList::TextureList(QWidget *parent)
    : QListWidget{parent}
{
    setMouseTracking(true);
    setIconSize(QSize{64, 64});
    setResizeMode(QListView::Adjust);
    setViewMode(QListView::IconMode);
    setUniformItemSizes(true);
    createContextMenu();
}

void TextureList::renameTriggered()
{
    assert(m_itemContextMenu != nullptr);
    const QString oldName = m_itemContextMenu->toolTip();
    const QString newName = processRenameDialog();
    if(!newName.isEmpty()){
        replaceTextureNameInList(newName);
        emit renameTileScene(oldName, newName);
        emit docModified();
    }
}

void TextureList::removeFromMapTriggered()
{
    assert(m_itemContextMenu != nullptr);
    // Connected with MapView::removeTileFromScene to execute command DeleteSelection
    emit removeTileFromScene(m_itemContextMenu->toolTip());
}

void TextureList::removeFromListTriggered()
{
    assert(m_itemContextMenu != nullptr);
    auto result = QMessageBox::warning(this,
                                       tr("Remove tile from list"),
                                       tr("Warning : Removing tile from list remove it from map too.\nAre you sure ?"),
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if(result == QMessageBox::Ok){
        // Connected with MainWindow::removeItemFromTextureList
        // MainWindow executes the remove item command
        removeFromMapTriggered();
        emit removeItemFromTextureList(m_itemContextMenu->toolTip());
    }
}

#ifndef QT_NO_CONTEXTMENU
void TextureList::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidgetItem *item = canShowContextMenu(event->pos());
    if(item != nullptr){
        m_itemContextMenu = item;
        m_contextMenu->exec(event->globalPos());
    }
    m_itemContextMenu = nullptr;
}
#endif // QT_NO_CONTEXTMENU

QListWidgetItem* TextureList::addTexture(const QString& fileName, bool fromLoadFile)
{
    const QString cuttedFileName = StringTools::cutFileName(fileName);
    const QString textureName = StringTools::cutExtensionFileName(cuttedFileName);

    if(!textureAlreadyExists(textureName)){
        QPixmap texture{fileName};
        if(fileName.isEmpty())
            throw(std::logic_error{"TextureList::addTexture() - filename empty - "});
        if(texture.isNull())
            throw(std::logic_error{"TextureList::addTexture() - texture null - " + fileName.toStdString()});
        m_textures[textureName] = texture;
        QPixmap scaledTexture = texture.scaled(iconSize());
        QIcon icon{scaledTexture};
        QListWidgetItem *item = new QListWidgetItem{icon, ""};
        item->setSizeHint(QSize{iconSize() + QSize{4, 4}});
        item->setToolTip(textureName);
        addItem(item);
        m_widgetItems.push_back(item);
        if(!fromLoadFile)
            emit docModified();
        return item;
    }
    return nullptr;
}

// Used when sprite sheet has been cutted
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
            m_widgetItems.push_back(item);
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

QListWidgetItem* TextureList::widgetItem(const QString& textureName) const
{
    for(const auto& item : m_widgetItems){
        if(item->toolTip() == textureName){
            return item;
        }
    }
    return nullptr;
}

QList<QListWidgetItem*> TextureList::widgetItems() const
{
    return m_widgetItems;
}

void TextureList::clean()
{
    clear();
    m_textures.clear();
    m_widgetItems.clear();
}

void TextureList::createContextMenu()
{
    m_contextMenu = new QMenu{this};

    m_renameAct = new QAction{tr("Rename tile"), this};
    m_contextMenu->addAction(m_renameAct);
    connect(m_renameAct, &QAction::triggered,
            this, &TextureList::renameTriggered);

    m_contextMenu->addSeparator();

    m_removeFromMapAct = new QAction{tr("Remove from map"), this};
    m_contextMenu->addAction(m_removeFromMapAct);
    connect(m_removeFromMapAct, &QAction::triggered,
            this, &TextureList::removeFromMapTriggered);

    m_removeFromListAct = new QAction{tr("Remove from list"), this};
    m_contextMenu->addAction(m_removeFromListAct);
    connect(m_removeFromListAct, &QAction::triggered,
            this, &TextureList::removeFromListTriggered);
}

bool TextureList::textureAlreadyExists(const QString &fileName, bool messageBoxActived)
{
    auto it = m_textures.find(fileName);
    if(it != std::end(m_textures)){
        if(messageBoxActived)
            QMessageBox::information(this, "Mapox2D",
                                     tr("Texture name already exists !"));
        return true;
    }
    if(fileName.isEmpty()){
        if(messageBoxActived)
            QMessageBox::information(this, "Mapox2D",
                                     tr("Texture name is empty !"));
        return true;
    }

    return false;
}

QListWidgetItem* TextureList::canShowContextMenu(const QPoint& viewPortPos)
{
    QListWidgetItem *item = itemAt(viewPortPos);
    if(item != nullptr){
        if(item->isSelected())
            return item;
    }
    return nullptr;
}

QString TextureList::processRenameDialog()
{
    // 2 Possibles output :
    //    - Empty QString
    //    - New Name

    QInputDialog dial{this};
    dial.setInputMode(QInputDialog::TextInput);
    dial.setTextValue(m_itemContextMenu->toolTip());
    dial.setLabelText(tr("Enter the new name :"));
    QString newName{""};
    do{
        dial.exec();
        newName = dial.textValue();
    }while(dial.result() != QDialog::Rejected && textureAlreadyExists(newName));
    if(dial.result() == QDialog::Rejected){
        newName = "";
    }
    return newName;
}

void TextureList::replaceTextureNameInList(const QString& newName)
{
    assert(m_itemContextMenu != nullptr && !newName.isEmpty());
    auto it = m_textures.find(m_itemContextMenu->toolTip());
    assert(it != std::end(m_textures));
    m_textures[newName] = it->second;
    m_textures.erase(it);
    m_itemContextMenu->setToolTip(newName);
}
