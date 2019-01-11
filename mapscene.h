#ifndef MAPSCENE_H
#define MAPSCENE_H

#include "texturelist.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QGraphicsTextItem>
#include <QStatusBar>
#include <QListWidgetItem>

class MapScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MapScene(QObject *parent = nullptr);

public:
    void holdStatusBar(QStatusBar* statusBar);
    void holdTextureList(TextureList* texturelist);
    void createMatrix(int tileWidth, int tileHeight, int rows, int cols);

public slots:
    // Connected with signal &QListWidget::itemClicked
    void currentTextureSelectedInList(QListWidgetItem* item);

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    int indexRelativeToMouse(const QPointF& mousePos);
    void fillTile(const QPointF& mousePos);

private:
    std::vector<QGraphicsRectItem*>   m_tiles{};
    std::vector<QGraphicsPixmapItem*> m_tilesTextures{};
    QString                           m_mousePosStr{};
    QStatusBar                       *m_statusBar{nullptr};
    TextureList                      *m_textureList{nullptr};
    QSize                             m_tileSize{};
    int                               m_rows{};
    int                               m_cols{};
    int                               m_currentIndex{};
    bool                              m_mouseLeftPress{false};
    QString                           m_currentTextureFileName{};
};

#endif // MAPSCENE_H
