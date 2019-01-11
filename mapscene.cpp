#include "mapscene.h"

MapScene::MapScene(QObject *parent)
    : QGraphicsScene{parent}
{

}

void MapScene::holdStatusBar(QStatusBar *statusBar)
{
    m_statusBar = statusBar;
}

void MapScene::holdTextureList(TextureList *texturelist)
{
    m_textureList = texturelist;
}

void MapScene::createMatrix(int tileWidth, int tileHeight, int rows, int cols)
{
    clear();
    m_tiles.clear();
    m_tilesTextures.clear();
    m_tileSize = QSize{tileWidth, tileHeight};
    m_rows = rows;
    m_cols = cols;
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            m_tiles.push_back(addRect(j*tileWidth, i*tileHeight, tileWidth, tileHeight));
            m_tilesTextures.push_back(addPixmap(QPixmap{}));
            m_tilesTextures.back()->setPos(j*tileWidth, i*tileHeight);
        }
    }
}

void MapScene::currentTextureSelectedInList(QListWidgetItem *item)
{
    m_currentTextureFileName = item->text();
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_mousePosStr = "x(" + QString::number(mouseEvent->scenePos().x(), 'f', 0) +
                    "), y(" +
                    QString::number(mouseEvent->scenePos().y(), 'f', 0) + ")";

    if(m_statusBar != nullptr)
        m_statusBar->showMessage(m_mousePosStr + " - index[" +
                                 QString::number(indexRelativeToMouse(mouseEvent->scenePos())) +
                                 "]");

    if(m_mouseLeftPress){
        if(indexRelativeToMouse(mouseEvent->scenePos()) != -1){
            fillTile(mouseEvent->scenePos());
        }
    }
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton){
        m_mouseLeftPress = true;
        if(indexRelativeToMouse(mouseEvent->scenePos()) != -1){
            fillTile(mouseEvent->scenePos());
        }
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

void MapScene::fillTile(const QPointF& mousePos)
{
    int index = indexRelativeToMouse(mousePos);
    if(!m_currentTextureFileName.isEmpty()){
        if(index != -1){
            QPixmap scaled = m_textureList->getTexture(m_currentTextureFileName).scaled(m_tileSize.width(), m_tileSize.height());
            m_tilesTextures[static_cast<std::size_t>(index)]->setPixmap(scaled);
        }
    } else {
        m_statusBar->showMessage("No texture selected !");
    }
}
