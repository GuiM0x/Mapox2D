#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "mapscene.h"
#include "tileitem.h"
#include "commands/filltilecommand.h"
#include "commands/deletetilecommand.h"
#include "commands/pastecommand.h"
#include "commands/fillselectioncommand.h"
#include "commands/anchorcommand.h"
#include "commands/deleteselectioncommand.h"
#include "commands/discardfloatselectioncommand.h"
#include "tools/utilitytools.h"

#include <map>
#include <algorithm>
#include <iterator>

#include <QDebug>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QStatusBar>
#include <QMouseEvent>
#include <QList>

class MapView : public QGraphicsView
{
    Q_OBJECT

    using ItemsSelected  = std::vector<std::tuple<TileItem*, QPen>>;
    using ItemsCopied    = QList<TileItem*>;
    using FloatSelection = QList<TileItem*>;

public:
    MapView(QWidget *parent = nullptr);

public:
    void holdStatusBar(QStatusBar *statusBar);
    void holdUndoStack(QUndoStack *undoStack);
    void reset(const QRectF& mapSceneBounding = QRectF{});

signals:
    void checkTool(ToolType type = ToolType::NoTool);
    void activeAnchorAct(bool actived);


public slots:
    // Connected with signal &MapScene::mouseMoveAndPressLeft
    // The signal indicate that a mouse Left button is pressed
    // and mouse moving. This slot create a command that fill a tile
    void mouseMovingAndPressing();
    void rubberChanged(const QRect& rubberBandRect);
    void toolTriggered(bool trigger, ToolType type);
    void copyTriggered();
    void cutTriggered();
    void pasteTriggered();
    void itemFocusChanged();
    void fillSelection();
    void anchorSelection();
    void undoActTriggered(bool trigger);
    void selectAll();
    // Connected with signal Texture::removeTileFromScene
    void removeTileFromScene(const QString& tileName);

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
    void shrinkToSelection(const QList<QGraphicsItem*>& itemsSelected);
    void growToSelection(const QList<QGraphicsItem*>& itemsSelected);
    TileItem* copyTile(TileItem *itemToCopy);
    QRectF pastedSelectionBoundingRect() const;
    bool canDragPastedSelection() const;
    bool canAnchor();
    void adjustFocusRectPastedSelectionDragging();
    void adjustPastedSelectionPosEndDrag();
    void movePastedSelection();
    void anchorPastedSelection();
    void copyItemsSelected();

private:
    std::map<std::string, bool> m_keysState{};
    double                      m_zoomFactor{1.0};
    QStatusBar                 *m_statusBar{nullptr};
    QUndoStack                 *m_undoStack{nullptr};
    ItemsSelected               m_originalItemsSelected{};
    ItemsCopied                 m_copiedItems{};
    FloatSelection              m_floatSelection{};
    bool                        m_selectionToolActived{false};
    bool                        m_moveSelectionToolActived{false};
    bool                        m_brushToolActived{false};

private:
    const char padding[5] = ""; // Only for padding "byte"
};

#endif // MAPVIEW_H
