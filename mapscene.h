#ifndef MAPSCENE_H
#define MAPSCENE_H

#include "texturelist.h"

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
#include <QProgressDialog>
#include <QElapsedTimer>

class MapScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MapScene(QObject *parent = nullptr);

public:
    void holdStatusBar(QStatusBar* statusBar);
    void holdTextureList(TextureList* texturelist);
    void createMatrix(int tileWidth, int tileHeight, int rows, int cols);
    int currentTile() const;
    bool canFillTile(int index) const;
    bool canDeleteTile(int index) const;
    bool isModified() const;
    int rows() const;
    int cols() const;
    int tileWidth() const;
    int tileHeight() const;
    std::vector<QString>* allTilesName();
    void fillTile(int index, const QString& textureName);
    void deleteTile(int index);
    QString currentTextureName() const;

signals:
    // Connected with slot &MapView::mouseMovingAndPressing
    // The slot create command for the undo/redo that fill a tile
    void mouseMoveAndPressLeft(MapScene* mapScene);
    void docModified();

public slots:
    // Connected with signal &QListWidget::itemClicked
    void currentTextureSelectedInList(QListWidgetItem* item);

private:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    int indexRelativeToMouse(const QPointF& mousePos);
    bool isTileTextureSameAsCurrentSelected(int index)const;
    void openLoadingDialog();
    void fillTile(int index);
    void clearAllContainers();

private:
    std::vector<QGraphicsRectItem*>   m_tiles{};
    std::vector<QGraphicsPixmapItem*> m_tilesTextures{};
    std::vector<QString>              m_tilesTexturesNames{};
    QPointF                           m_mousePos{};
    QString                           m_mousePosStr{};
    QStatusBar                       *m_statusBar{nullptr};
    TextureList                      *m_textureList{nullptr};
    QSize                             m_tileSize{};
    int                               m_rows{};
    int                               m_cols{};
    int                               m_currentIndex{};
    bool                              m_mouseLeftPress{false};
    QString                           m_currentTextureFileName{};
    bool                              m_modified{false};
};

#endif // MAPSCENE_H
