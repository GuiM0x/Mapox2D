#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mapview.h"
#include "mapscene.h"
#include "texturelist.h"
#include "dialogs/newmapdialog.h"
#include "dialogs/importspritesheetdialog.h"
#include "tools/datasaver.h"
#include "tools/dataloader.h"
#include "tools/utilitytools.h"

#include <map>

#include <QDebug>
#include <QMainWindow>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QCloseEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QMenuBar>
#include <QListWidget>
#include <QFileDialog>
#include <QPixmap>
#include <QUndoView>
#include <QUndoStack>
#include <QToolBar>
#include <QDockWidget>
#include <QShowEvent>
#include <QDesktopWidget>
#include <QSizeGrip>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void toolTriggered(bool trigger, ToolType type);

private slots:
    void newMap();
    void open();
    bool save();
    bool saveAs();
    void docWasModified();
    void about();
    void openTexture();
    void openSpriteSheet();
    void quit();
    void selectToolTriggered(bool trigger);
    void moveSelectionToolTriggered(bool trigger);
    void brushTileToolTriggered(bool trigger);
    void checkedTool(ToolType type = ToolType::NoTool);
    void replaceTextureList(bool replace);
    void replaceUndoView(bool replace);
    void clearUndoStack();

private:
    void createActions();
    void createMapView();
    void createMapScene();
    void createMapScene(std::map<NewMapDialog::FieldType, int>& values);
    void createStatusBar();
    void createUndoView();
    void createDockWindows();

    bool maybeSave();
    bool saveFile(const QString& fileName);
    void setCurrentFile(const QString& fileName);
    void loadFile(const QString& fileName);

private:
    MapView        *m_mapView{nullptr};
    MapScene       *m_mapScene{nullptr};
    TextureList    *m_textureList{nullptr};
    QUndoStack     *m_undoStack{nullptr};
    QUndoView      *m_undoView{nullptr};
    bool            m_mouseLeftPress{false};
    QString         m_currentFile{};
    bool            m_documentModified{false};
    QAction        *m_selectAct{nullptr};
    QAction        *m_moveSelectionAct{nullptr};
    QAction        *m_brushTileAct{nullptr};
    QMenu          *m_viewMenu{nullptr}; // View/Hide dock widget
    QDockWidget    *m_dockTextureList{nullptr};
    QDockWidget    *m_dockUndoView{nullptr};
};

#endif // MAINWINDOW_H
