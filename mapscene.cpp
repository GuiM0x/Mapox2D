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

    QProgressDialog progress{"Creating tiles...", "Cancel", 0, 100};
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.show();

    int steps{0};
    const int totalTiles = rows*cols;

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

            // If 1% of totalTiles >= 1 tile
            if(totalTiles/100.f >= 1){
                // Check if current index >= 1% of total * current step
                if((i*cols+j) >= (totalTiles/100.f)*steps+1){
                    // 1 step = 1%
                    ++steps;
                    progress.setValue(steps);
                }
            } else {
                // Display % relative to current tile
                progress.setValue(static_cast<int>((i*cols+j)/(totalTiles/100.f)));
            }
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

void MapScene::loadMap(const QString& datas)
{
    qDebug() << "Datas loaded = " + datas;
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

void MapScene::clearAllContainers()
{
    clear();
    m_tiles.clear();
    m_tilesTextures.clear();
    m_tilesTexturesNames.clear();
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
