#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QGraphicsTextItem>
#include <QStatusBar>

class MapScene : public QGraphicsScene
{
public:
    MapScene(QObject *parent = nullptr);

public:
    void holdStatusBar(QStatusBar* statusBar);
    void createMatrix(int tileWidth, int tileHeight, int rows, int cols);

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    int indexRelativeToMouse(const QPointF& mousePos);
    void fillTile(const QPointF& mousePos);

private:
    std::vector<QGraphicsRectItem*>   m_tiles{};
    std::vector<QGraphicsPixmapItem*> m_textures{};
    QString                           m_mousePosStr{};
    QStatusBar                       *m_statusBar{nullptr};
    QSize                             m_tileSize{};
    int                               m_rows{};
    int                               m_cols{};
    int                               m_currentIndex{};
    bool                              m_mouseLeftPress{false};
};

#endif // MAPSCENE_H
