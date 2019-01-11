#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "newmapdialog.h"
#include "mapview.h"
#include "mapscene.h"
#include "texturelist.h"

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
    void createActions();
    void createMapView();
    void createMapScene();
    void createMapScene(std::map<QString, int>& values);
    void createGridLayout();
    void createStatusBar();
    void maybeSave();

private:
    QWidget     *m_centralWidget{nullptr};
    MapView     *m_mapView{nullptr};
    MapScene    *m_mapScene{nullptr};
    TextureList *m_textureList{nullptr};
    QPushButton *m_addTextureButton{nullptr};
};

#endif // MAINWINDOW_H
