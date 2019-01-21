#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "mapscene.h"
#include "commands/filltilecommand.h"
#include "commands/deletetilecommand.h"

#include <map>
#include <algorithm>
#include <iterator>

#include <QDebug>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QStatusBar>
#include <QMouseEvent>
#include <QClipboard>
#include <QMimeData>

class MapView : public QGraphicsView
{
    Q_OBJECT

    using ItemsSelected = std::vector<std::tuple<QGraphicsRectItem*, QPen>>;

public:
    MapView(QWidget *parent = nullptr);

public:
    void holdUndoStack(QUndoStack *undoStack);
    void reset(const QRectF& mapSceneBounding);

public slots:
    // Connected with signal &MapScene::mouseMoveAndPressLeft
    // The signal indicate that a mouse Left button is pressed
    // and mouse moving. This slot create a command that fill a tile
    void mouseMovingAndPressing();
    void rubberChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);
    void selectToolActived(bool actived);
    void copyTriggered();
    void pasteTriggered();

private:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void createKeys();
    void restoreOriginalSeletedItem();
    void fillTile();

private:
    std::map<std::string, bool> m_keysState{};
    double                      m_zoomFactor{1.0};
    QUndoStack                 *m_undoStack{nullptr};
    ItemsSelected               m_previousItemsSelected{};
    bool                        m_selectionToolActived{false};

private:
    const char padding[7] = ""; // Only for padding "byte"
};

#endif // MAPVIEW_H
