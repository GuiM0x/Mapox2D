#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "newmapdialog.h"
#include "mapview.h"
#include "mapscene.h"
#include "texturelist.h"
#include "filltilecommand.h"
#include "mycentrawidget.h"

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newMap();
    void open();
    void save();
    void saveAs();
    void docWasModified();
    void about();
    void openTexture();

private:
    void mousePressEvent(QMouseEvent *event) override;
    // MOUSE MOVE EVENT NOT WORKING !!
    // CREATE CUSTOM CENTRAL WIDGET AND OVERRIDE IN IT ?
    // void mouseMoveEvent(QMouseEvent *event) override;

private:
    void createActions();
    void createMapView();
    void createMapScene();
    void createMapScene(std::map<QString, int>& values);
    void createGridLayout();
    void createStatusBar();
    void createUndoView();
    void maybeSave();

private:
    MyCentraWidget *m_centralWidget{nullptr};
    MapView        *m_mapView{nullptr};
    MapScene       *m_mapScene{nullptr};
    TextureList    *m_textureList{nullptr};
    QPushButton    *m_addTextureButton{nullptr};
    QUndoStack     *m_undoStack{nullptr};
    QUndoView      *m_undoView{nullptr};
    bool            m_mouseLeftPress{false};

};

#endif // MAINWINDOW_H
