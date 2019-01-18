#include "mapscene.h"

MapScene::MapScene(QObject *parent)
    : QGraphicsScene{parent}
{

}

void MapScene::holdStatusBar(QStatusBar *statusBar)
{
    if(m_statusBar == nullptr)
        m_statusBar = statusBar;
}

void MapScene::holdTextureList(TextureList *texturelist)
{
    if(m_textureList == nullptr)
        m_textureList = texturelist;
}

void MapScene::createMatrix(int tileWidth, int tileHeight, int rows, int cols)
{
    clearAllContainers();

    m_tileSize = QSize{tileWidth, tileHeight};
    m_rows = rows;
    m_cols = cols;
    const int totalTiles = rows*cols;
    LoadingMapDialog progress{totalTiles, "Creating tiles...", "Cancel", 0, 100};

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            m_tiles.push_back(addRect(j*tileWidth, i*tileHeight, tileWidth, tileHeight));
            m_tilesTextures.push_back(addPixmap(QPixmap{}));
            m_tilesTextures.back()->setPos(j*tileWidth, i*tileHeight);
            m_tilesTexturesNames.push_back("");
            if(progress.wasCanceled()){
                clearAllContainers();
                break;
            }
            progress.updateBar(i*cols+j);
        }
    }
    progress.setValue(100);
}

int MapScene::currentTile() const
{
    return m_currentIndex;
}

bool MapScene::canFillTile(int index) const
{
    if(index != -1 &&
            m_textureList != nullptr &&
            !m_currentTextureFileName.isEmpty() &&
            !isTileTextureSameAsCurrentSelected(index)){
        return true;
    }
    return false;
}

bool MapScene::canDeleteTile(int index) const
{
    if(index != -1 &&
            m_tilesTexturesNames[static_cast<std::size_t>(index)] != ""){
        return true;
    }
    return false;
}

bool MapScene::isModified() const
{
    return m_modified;
}

int MapScene::rows() const
{
    return m_rows;
}

int MapScene::cols() const
{
    return m_cols;
}

int MapScene::tileWidth() const
{
    return m_tileSize.width();
}

int MapScene::tileHeight() const
{
    return m_tileSize.height();
}

std::vector<QString>* MapScene::allTilesName()
{
    return &m_tilesTexturesNames;
}

// USED BY COMMANDS
void MapScene::fillTile(int index, const QString& textureName)
{
    if(index != -1 && m_textureList != nullptr){
        std::size_t id = static_cast<std::size_t>(index);
        if(textureName != m_tilesTexturesNames[id]){
            QPixmap scaled = m_textureList->getTexture(textureName).scaled(m_tileSize.width(), m_tileSize.height());
            m_tilesTextures[id]->setPixmap(scaled);
            m_tilesTexturesNames[id] = textureName;
        }
    }
}

// USED BY COMMANDS
void MapScene::deleteTile(int index)
{
    if(index != -1){
        std::size_t id = static_cast<std::size_t>(index);
        m_tilesTexturesNames[id] = "";
        m_tilesTextures[id]->setPixmap(QPixmap{});
    }
}

QString MapScene::currentTextureName() const
{
    return m_currentTextureFileName;
}

// USED BY COMMANDS
std::vector<QString> MapScene::fillAll(const QString& textureName)
{
    const std::vector<QString> oldTilesTexturesNames{m_tilesTexturesNames};
    const int gridSize = m_rows*m_cols;
    LoadingMapDialog progress{gridSize, "Filling map...", "Cancel", 0, 100};
    for(int i = 0; i < gridSize; ++i){
        fillTile(i, textureName);
        progress.updateBar(i);
    }
    progress.setValue(100);
    return oldTilesTexturesNames;
}

// USED BY COMMANDS
void MapScene::fillAll(const std::vector<QString>& oldTilesTexturesNames)
{
    const int gridSize = m_rows*m_cols;
    LoadingMapDialog progress{gridSize, "Filling map...", "Cancel", 0, 100};
    for(std::size_t i = 0; i < oldTilesTexturesNames.size(); ++i){
        if(!oldTilesTexturesNames[i].isEmpty()){
            fillTile(static_cast<int>(i), oldTilesTexturesNames[i]);
        } else {
            m_tilesTextures[i]->setPixmap(QPixmap{});
            m_tilesTexturesNames[i] = "";
        }
        progress.updateBar(static_cast<int>(i));
    }
    progress.setValue(100);
}

void MapScene::currentTextureSelectedInList(QListWidgetItem *item)
{
    m_currentTextureFileName = item->text();
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_mousePos = mouseEvent->scenePos();
    m_mousePosStr = "x(" + QString::number(mouseEvent->scenePos().x(), 'f', 0) +
                    "), y(" + QString::number(mouseEvent->scenePos().y(), 'f', 0) + ")";

    m_currentIndex = indexRelativeToMouse(mouseEvent->scenePos());

    if(m_statusBar != nullptr)
        m_statusBar->showMessage(m_mousePosStr + " - index[" +
                                 QString::number(m_currentIndex) +
                                 "]");
    if(m_mouseLeftPress)
        emit mouseMoveAndPressLeft(this);
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton){
        m_mouseLeftPress = true;
    }
}

void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton){
        m_mouseLeftPress = false;
    }
}

int MapScene::indexRelativeToMouse(const QPointF& mousePos)
{
    if(mousePos.x() < 0 ||
            mousePos.y() < 0 ||
            mousePos.x() >= m_cols * m_tileSize.width() ||
            mousePos.y() >= m_rows * m_tileSize.height()){
        m_currentIndex = -1;
    }
    else {
        int i = static_cast<int>(mousePos.y() / m_tileSize.height());
        int j = static_cast<int>(mousePos.x() / m_tileSize.width());
        m_currentIndex = (i * m_cols) + j;
    }

    return m_currentIndex;
}

bool MapScene::isTileTextureSameAsCurrentSelected(int index) const
{
    if(index != -1){
        std::size_t id = static_cast<std::size_t>(index);
        if(m_tilesTexturesNames[id] == m_currentTextureFileName)
            return true;
    } else {
        return true;
    }
    return false;
}

void MapScene::fillTile(int index)
{
    if(canFillTile(index)){
        std::size_t id = static_cast<std::size_t>(index);
        QPixmap scaled = m_textureList->getTexture(m_currentTextureFileName).scaled(m_tileSize.width(), m_tileSize.height());
        m_tilesTextures[id]->setPixmap(scaled);
        m_tilesTexturesNames[id] = m_currentTextureFileName;
    }
}

void MapScene::clearAllContainers()
{
    clear();
    m_tiles.clear();
    m_tilesTextures.clear();
    m_tilesTexturesNames.clear();
}
