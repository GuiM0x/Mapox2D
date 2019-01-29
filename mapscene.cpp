#include "mapscene.h"

MapScene::MapScene(QObject *parent)
    : QGraphicsScene{parent}
{
    connect(this, &MapScene::itemFocusChange, this, &MapScene::itemFocusChanged);
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
            // Warning : When add an item in scene,
            //           always init with pos(0, 0) and
            //           then setPos(x, y) on item.
            //           Because in function add...(), the pos
            //           represents the draw coordinate, not the items's pos.
            //           setPos(x, y) set position for drawing and item coordinates
            qreal width = tileWidth;
            qreal height = tileHeight;
            const QRectF boundings{QPointF{0, 0}, QSizeF{width, height}};
            TileItem *tile = new TileItem{boundings};
            QPen pen{};
            pen.setBrush(QBrush{QColor{135, 135, 135}});
            tile->setPen(pen);
            qreal x = j*tileWidth;
            qreal y = i*tileHeight;
            tile->setPos(x, y);
            tile->setName("");
            tile->setIndex(i*cols+j);
            m_tilesTexturesNames.push_back("");
            m_tiles.push_back(tile);
            addItem(tile);
            if(progress.wasCanceled()){
                clearAllContainers();
                break;
            }
            progress.updateBar(i*cols+j);
        }
    }
    progress.setValue(100);
    createFocusRect(tileWidth, tileHeight);
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
            m_tiles[static_cast<std::size_t>(index)]->name() != ""){
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
            m_tiles[id]->setBrush(QBrush{scaled});
            m_tiles[id]->setName(textureName);
            m_tilesTexturesNames[id] = textureName;
        }
    }
}

// USED BY COMMANDS
void MapScene::deleteTile(int index)
{
    if(index != -1){
        std::size_t id = static_cast<std::size_t>(index);
        m_tiles[id]->setBrush(QBrush{QPixmap{}});
        m_tiles[id]->setName("");
        m_tilesTexturesNames[id] = "";
    }
}

int MapScene::currentTile() const
{
    return m_currentIndex;
}

QString MapScene::currentTextureName() const
{
    return m_currentTextureFileName;
}

QString MapScene::currentTileName() const
{
    if(m_currentIndex != -1)
        return m_tiles[static_cast<std::size_t>(m_currentIndex)]->name();
    return "";
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
            m_tiles[i]->setBrush(QBrush{QPixmap{}});
            m_tiles[i]->setName("");
            m_tilesTexturesNames[i] = "";
        }
        progress.updateBar(static_cast<int>(i));
    }
    progress.setValue(100);
}

QPointF MapScene::mousePosition() const
{
    return m_mousePos;
}

QPointF MapScene::mouseMoveVector() const
{
    const QPointF moveVector{m_mousePos - m_mouseLastPos};
    return moveVector;
}

QPointF MapScene::focusRectPos() const
{
    const QPointF pos = (m_focusRect == nullptr) ? QPointF{} : m_focusRect->scenePos();
    return pos;
}

TileItem* MapScene::itemByIndex(int index)
{
    if(index < 0 || index >= static_cast<int>(m_tiles.size()))
        throw(std::logic_error{"MapScene::itemByIndex - index Out of range"});
    return m_tiles[static_cast<std::size_t>(index)];
}

QGraphicsRectItem* MapScene::focusRect()
{
    assert(m_focusRect != nullptr && "MapScene::focusRect - m_focusRect cannot be null");
    return m_focusRect;
}

void MapScene::currentTextureSelectedInList(QListWidgetItem *item)
{
    m_currentTextureFileName = item->text();
}

void MapScene::itemFocusChanged()
{
    if(m_focusRect != nullptr && m_currentIndex != -1){
        if(m_tiles[static_cast<std::size_t>(m_currentIndex)] != nullptr){
            QPointF pos = m_tiles[static_cast<std::size_t>(m_currentIndex)]->pos();
            m_focusRect->setPos(pos);
        }
    }
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_mouseLastPos = m_mousePos;
    m_mousePos = mouseEvent->scenePos();
    m_mousePosStr = "x(" + QString::number(mouseEvent->scenePos().x(), 'f', 0) +
                    "), y(" + QString::number(mouseEvent->scenePos().y(), 'f', 0) + ")";

    const int oldIndex = m_currentIndex;
    const int newIndex = indexRelativeToPos(m_mousePos);
    m_currentIndex = newIndex;
    if(newIndex != oldIndex){
        emit itemFocusChange();
    }

    if(m_statusBar != nullptr){
        QString tileName =
                (m_currentIndex != -1) ? m_tiles[static_cast<std::size_t>(m_currentIndex)]->name() : "No Tile";
        if(tileName.isEmpty()) tileName = "Default";
        m_statusBar->showMessage(m_mousePosStr + " - index[" +
                                 QString::number(m_currentIndex) +
                                 "] = " + tileName);
    }

    if(m_mouseLeftPress)
        emit mouseMoveAndPressLeft();
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

int MapScene::indexRelativeToPos(const QPointF& pos)
{
    if(pos.x() < 0 ||
            pos.y() < 0 ||
            pos.x() >= m_cols * m_tileSize.width() ||
            pos.y() >= m_rows * m_tileSize.height()){
        m_currentIndex = -1;
    }
    else {
        int i = static_cast<int>(pos.y() / m_tileSize.height());
        int j = static_cast<int>(pos.x() / m_tileSize.width());
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
        m_tiles[id]->setBrush(QBrush{scaled});
        m_tiles[id]->setName(m_currentTextureFileName);
        m_tilesTexturesNames[id] = m_currentTextureFileName;
    }
}

void MapScene::clearAllContainers()
{
    clear();
    m_tiles.clear();
    m_tilesTexturesNames.clear();
    m_focusRect = nullptr;
}

void MapScene::createFocusRect(int tileWidth, int tileHeight)
{
    qreal width = tileWidth;
    qreal height = tileHeight;
    const QRectF boundings{QPointF{0, 0}, QSizeF{width, height}};
    if(m_focusRect == nullptr)
        m_focusRect = new TileItem{boundings};
    addItem(m_focusRect);
    m_focusRect->setBrush(QBrush{QColor{25, 110, 250, 30}});
    m_focusRect->setPen(QPen{Qt::NoPen});
    m_focusRect->setFlag(QGraphicsItem::ItemIsSelectable, false);
}
