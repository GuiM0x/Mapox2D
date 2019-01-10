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

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

public:
    QSize m_tileSize{};

private:
    QString m_mousePosStr{};
    QStatusBar *m_statusBar{nullptr};
};

#endif // MAPSCENE_H
