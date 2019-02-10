#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mapview.h"
#include "mapscene.h"
#include "texturelist.h"
#include "dialogs/newmapdialog.h"
#include "dialogs/importspritesheetdialog.h"
#include "commands/removeitemfromtexturelistcommand.h"
#include "tools/datasaver.h"
#include "tools/dataloader.h"
#include "tools/utilitytools.h"
#include "tools/stylesheetregister.h"

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
#include <QColorDialog>

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
    // Connected with signal TextureList::removeItemFromTextureList
    void removeItemFromTextureList(const QString& textureName);
    void changeBackgroundViewColor();
    void changeGridColor();

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
    void registerStyleSheet();

private:
    // StyleSheetRegister Class Infos :
    // ================================
    //    - Inherits : Singleton class
    //    - Instancied with static function Singleton<T>::createInstance(Args ...args)
    //        -> The parameter pack is passed to the constructor of
    //           T when it's instancied
    //    - In this case, StyleSheetRegister has only one constructor
    //        -> StyleSheetRegister(const QString& folderName)
    //        -> So we pass the folder's name to createInstance(...)'s parameters.
    //    - Then, to register a css file in the class, simply call :
    //        -> StyleSheetRegister::registerStyleSheet(const QString& filename, StyleSheet style)
    //        -> StyleSheet is just an enum
    //        -> To add new StyleSheet, add it to the StyleSheet enum in StyleSheetRegister
    //    - To apply a style sheet to a widget, use the overload operator [] :
    //        -> a_widget->setStyleSheet(m_styleRegitered[StyleSheet::CustomStyle])
    //
    // [Warning] : all "*.css" files must be registered in a ".qrc" file
    StyleSheetRegister m_styleRegister =
        Singleton<StyleSheetRegister>::createInstance("stylesheets");

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
