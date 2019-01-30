#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mapview.h"
#include "mapscene.h"
#include "texturelist.h"
#include "commands/fillallcommand.h"
#include "dialogs/newmapdialog.h"
#include "tools/datasaver.h"
#include "tools/dataloader.h"
#include "tools/utilitytools.h"

#include <map>

#include <QDebug>
#include <QMainWindow>
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void selectToolActived(bool actived);
    void moveSelectionToolActived(bool actived);
    void brushTileToolActived(bool checked);

private slots:
    void newMap();
    void open();
    bool save();
    bool saveAs();
    void docWasModified();
    void about();
    void openTexture();
    void fillAll();
    void quit();
    void selectToolChecked(bool checked = false);
    void moveSelectionToolChecked(bool checked = false);
    void brushTileToolChecked(bool checked = false);
    void uncheckSelectAct();
    void uncheckMoveSelectionAct();
    void uncheckBrushToolAct();
    void checkSelectAct();
    void checkMoveSelectionAct();
    void checkBrushToolAct();

private:
    void createActions();
    void createMapView();
    void createMapScene();
    void createMapScene(std::map<QString, int>& values);
    void createGridLayout();
    void createStatusBar();
    void createUndoView();

    bool maybeSave();
    bool saveFile(const QString& fileName);
    void setCurrentFile(const QString& fileName);
    void loadFile(const QString& fileName);

private:
    QWidget        *m_centralWidget{nullptr};
    MapView        *m_mapView{nullptr};
    MapScene       *m_mapScene{nullptr};
    TextureList    *m_textureList{nullptr};
    QPushButton    *m_addTextureButton{nullptr};
    QUndoStack     *m_undoStack{nullptr};
    QUndoView      *m_undoView{nullptr};
    bool            m_mouseLeftPress{false};
    QString         m_currentFile{};
    bool            m_documentModified{false};

    QAction        *m_selectAct{nullptr};
    QAction        *m_moveSelectionAct{nullptr};
    QAction        *m_brushTileAct{nullptr};
};

#endif // MAINWINDOW_H
