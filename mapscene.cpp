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
    m_textureList = texturelist;
    assert(m_textureList != nullptr);
    connect(m_textureList, &TextureList::renameTileScene, this, &MapScene::renameTile);
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
            qreal x = j*tileWidth;
            qreal y = i*tileHeight;
            createTile(x, y, tileWidth, tileHeight);
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

// USED BY COMMANDS (FillTileCommand)
void MapScene::fillTile(int index, const QString& textureName, bool isUndoCommand)
{
    // Note : isUndoCommand is here to avoid new texture created in list
    //        when FillTileCommand::undo() is called

    assert(index >= 0 && index < m_rows*m_cols);

    auto tile = itemByIndex(index);
    QImage actualImage = tile->brush().textureImage();
    const QString actualName = tile->name();

    QPixmap scaled{};
    if(!textureName.isEmpty())
        scaled = m_textureList->getTexture(textureName)
                 .scaled(m_tileSize.width(), m_tileSize.height());
    //assert(!scaled.isNull());
    QImage newImage = scaled.toImage();

    QString newName = textureName;
    if(!textureName.isEmpty()){
        QPainter painter{&newImage};
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        painter.drawImage(0, 0, actualImage);
        if(!actualName.isEmpty() && !isUndoCommand){ // Ok we've got a composed image
            newName = actualName + textureName;
            // add the composed brush to texturelist
            if(!m_textureList->textureAlreadyExists(newName, false)){
                auto listWidgetItem = m_textureList->addTexture(QBrush{newImage}, newName);
                listWidgetItem->setHidden(true);
            }
        }
    }

    const std::size_t id = static_cast<std::size_t>(index);
    QBrush brush{newImage};
    m_tiles[id]->setBrush(brush);
    m_tiles[id]->setName(newName);
    m_tilesTexturesNames[id] = newName;
}

// USED BY COMMANDS (FillSelectionCommand)
void MapScene::fillTile(TileItem *item, const QString& textureName)
{
    assert(item != nullptr);
    fillTile(item->index(), textureName);
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

const std::vector<TileItem*>* MapScene::tiles() const
{
    return &m_tiles;
}

void MapScene::resizeGrid(int rows, int cols)
{
    qDebug() << "MapScene::resizeGrid - Please wait...";

    if(cols < 0 && abs(cols) < m_cols) reduceCol(abs(cols));
    if(cols > 0)                       expandCol(cols);
    if(rows < 0 && abs(rows) < m_rows) reduceRow(abs(rows));
    if(rows > 0)                       expandRow(rows);

    // TO DO : don't forget to update m_rows and m_cols
}

void MapScene::currentTextureSelectedInList(QListWidgetItem *item)
{
    m_currentTextureFileName = item->toolTip();
}

void MapScene::renameTile(const QString& oldName, const QString& newName)
{
    assert(!oldName.isEmpty() && !newName.isEmpty());
    m_currentTextureFileName = newName;
    for(auto& name : m_tilesTexturesNames){
        if(name == oldName){
            name = newName;
        }
    }
    for(auto& tile : m_tiles){
        if(tile->name() == oldName){
            tile->setName(newName);
        }
    }
    emit clearUndoStack();
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
                (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_tiles.size())) ?
                    m_tiles[static_cast<std::size_t>(m_currentIndex)]->name() : "No Tile";
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
    if(index > -1){
        std::size_t id = static_cast<std::size_t>(index);
        //      Compare textureOnMap and textureInList end characters.
        //      Because when image are composed (few images stacked on the same tile),
        //      the name automatically concatened (tileOnMapName + tileSelectedInList).
        //      So, to avoid an infinite filling action, we need to compare end characters
        //      in these 2 string.
        //      To do that, we use algo' with reverse iterator.
        const QString textureOnMap  = m_tilesTexturesNames[id];
        const QString textureInList = m_currentTextureFileName;
        QString smallerString = textureOnMap;
        QString biggerString = textureInList;
        if(smallerString.size() != biggerString.size()){
            if(smallerString.size() > biggerString.size()){
                smallerString = textureInList;
                biggerString = textureOnMap;
            }
        }
        bool isEndCharacterEqual = std::equal(std::rbegin(smallerString), std::rend(smallerString),
                                              std::rbegin(biggerString));
        if(isEndCharacterEqual && !textureOnMap.isEmpty())
            return true;

    } else {
        return true;
    }
    return false;
}

void MapScene::fillTile(int index)
{
    if(canFillTile(index)){
        fillTile(index, m_currentTextureFileName);
    }
}

void MapScene::clearAllContainers()
{
    m_tiles.clear();
    m_tilesTexturesNames.clear();
    clear();
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

TileItem* MapScene::createTile(qreal x, qreal y,
                               int tileWidth, int tileHeight,
                               const QString& textureName,
                               const QBrush brush)
{
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
    tile->setPos(x, y);
    tile->setName(textureName);
    tile->setIndex(static_cast<int>(m_tiles.size()));
    tile->setBrush(brush);
    m_tilesTexturesNames.push_back(textureName);
    m_tiles.push_back(tile);
    addItem(tile);
    return tile;
}

void MapScene::reduceCol(int nbToReduce)
{
    assert(nbToReduce >= 0);
    std::vector<TileItem*> tilesToCopy{};
    for(int i = 0; i < m_rows; ++i){
        for(int j = 0; j < m_cols - nbToReduce; ++j){
            auto tile = m_tiles[static_cast<std::size_t>(i*m_cols+j)];
            if(!tile->name().isEmpty()){
                tile->setIndex(tile->index() - (i*nbToReduce));
                tilesToCopy.push_back(UtilityTools::copyTile(tile,
                                                             QSize{m_tileSize.width(), m_tileSize.height()}));
            }
        }
    }
    createMatrix(m_tileSize.width(), m_tileSize.height(),
                 m_rows, m_cols - nbToReduce);
    for(const auto& tile : tilesToCopy){
        fillTile(tile->index(), tile->name());
    }
}

void MapScene::expandCol(int nbToExpand)
{
    assert(nbToExpand >= 0);
    std::vector<TileItem*> tilesToCopy{};
    for(int i = 0; i < m_rows; ++i){
        for(int j = 0; j < m_cols; ++j){
            auto tile = m_tiles[static_cast<std::size_t>(i*m_cols+j)];
            if(!tile->name().isEmpty()){
                tile->setIndex(tile->index() + (i*nbToExpand));
                tilesToCopy.push_back(UtilityTools::copyTile(tile,
                                                             QSize{m_tileSize.width(), m_tileSize.height()}));
            }
        }
    }
    createMatrix(m_tileSize.width(), m_tileSize.height(),
                 m_rows, m_cols + nbToExpand);
    for(const auto& tile : tilesToCopy){
        fillTile(tile->index(), tile->name());
    }
}

void MapScene::reduceRow(int nbToReduce)
{
    assert(nbToReduce >= 0);
    const int totalToRemove = nbToReduce * m_cols;
    for(int i = 0; i < totalToRemove; ++i){
        removeItem(m_tiles.back());
        m_tiles.pop_back();
        m_tilesTexturesNames.pop_back();
    }
    m_rows -= nbToReduce;
}

void MapScene::expandRow(int nbToExpand)
{
    assert(nbToExpand >= 0);
    for(int i = 0; i < nbToExpand; ++i){
        for(int j = 0; j < m_cols; ++j){
            qreal x = j * m_tileSize.width();
            qreal y = m_rows * m_tileSize.height();
            createTile(x, y, m_tileSize.width(), m_tileSize.height());
        }
        ++m_rows;
    }
}
