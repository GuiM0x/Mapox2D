#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "mapscene.h"
#include "commands/filltilecommand.h"
#include "commands/deletetilecommand.h"

#include <map>

#include <QDebug>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QStatusBar>
#include <QMouseEvent>

class MapView : public QGraphicsView
{
    Q_OBJECT

public:
    MapView(QWidget *parent = nullptr);

public:
    void holdUndoStack(QUndoStack *undoStack);
    void reset(const QRectF& mapSceneBounding);

public slots:
    // Connected with signal &MapScene::mouseMoveAndPressLeft
    // The signal indicate that a mouse Left button is pressed
    // and mouse moving. This slot create a command that fill a tile
    void mouseMovingAndPressing(MapScene* mapScene);

private:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void createKeys();

private:
    std::map<std::string, bool> m_keysState{};
    double                      m_zoomFactor{1.0};
    QUndoStack                  *m_undoStack{nullptr};
};

#endif // MAPVIEW_H
