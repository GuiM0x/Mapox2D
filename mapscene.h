#ifndef MAPSCENE_H
#define MAPSCENE_H

#include "texturelist.h"
#include "tileitem.h"
#include "dialogs/loadingmapdialog.h"
#include "tools/utilitytools.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QGraphicsTextItem>
#include <QStatusBar>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QUndoStack>
#include <QUndoCommand>
#include <QProgressBar>
#include <QPainter>

class MapScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MapScene(QObject *parent = nullptr);

public:
    void holdStatusBar(QStatusBar* statusBar);
    void holdTextureList(TextureList* texturelist);
    void createMatrix(int tileWidth, int tileHeight, int rows, int cols);
    bool canFillTile(int index) const;
    bool canDeleteTile(int index) const;
    bool isModified() const;
    int rows() const;
    int cols() const;
    int tileWidth() const;
    int tileHeight() const;
    std::vector<QString>* allTilesName();
    void fillTile(int index, const QString& textureName, bool isUndoCommand = false);
    void fillTile(TileItem* item, const QString& textureName);
    void deleteTile(int index);
    int currentTile() const;
    QString currentTextureName() const;
    QString currentTileName() const;
    QPointF mousePosition() const;
    QPointF mouseMoveVector() const;
    QPointF focusRectPos() const;
    TileItem* itemByIndex(int index);
    QGraphicsRectItem* focusRect();
    const std::vector<TileItem*>* tiles() const;

signals:
    // Connected with slot &MapView::mouseMovingAndPressing
    // The slot create command for the undo/redo that fill a tile
    void mouseMoveAndPressLeft();
    void docModified();
    void itemFocusChange();
    void triggerTool(bool trigger = true,
                     ToolType type = ToolType::Selection);
    void clearUndoStack();

public slots:
    // Connected with signal &QListWidget::itemClicked
    void currentTextureSelectedInList(QListWidgetItem* item);
    // Triggered by Texture List
    void renameTile(const QString& oldName, const QString& newName);

private slots:
    void itemFocusChanged();

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    int indexRelativeToPos(const QPointF& pos);
    bool isTileTextureSameAsCurrentSelected(int index)const;
    void openLoadingDialog();
    void fillTile(int index);
    void clearAllContainers();
    void createFocusRect(int tileWidth, int tileHeight);
    TileItem* createTile(qreal x, qreal y,
                         int tileWidth, int tileHeight,
                         const QString& textureName = "",
                         const QBrush brush = QBrush{});

private:
    std::vector<TileItem*>            m_tiles{};
    std::vector<QString>              m_tilesTexturesNames{};
    QPointF                           m_mouseLastPos{};
    QPointF                           m_mousePos{};
    QString                           m_mousePosStr{};
    QStatusBar                       *m_statusBar{nullptr};
    TextureList                      *m_textureList{nullptr};
    QSize                             m_tileSize{};
    int                               m_rows{};
    int                               m_cols{};
    int                               m_currentIndex{-1};
    bool                              m_mouseLeftPress{false};
    QString                           m_currentTextureFileName{};
    bool                              m_modified{false};
    QGraphicsRectItem                *m_focusRect{nullptr};
    QGraphicsRectItem                *m_pasteSelectionRect{nullptr};
};

#endif // MAPSCENE_H
