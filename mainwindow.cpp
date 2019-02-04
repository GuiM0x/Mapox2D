#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      m_mapView{new MapView{this}},
      m_mapScene{new MapScene{m_mapView}},
      m_textureList{new TextureList{this}},
      m_undoStack{new QUndoStack{this}}
{
    setCentralWidget(m_mapView);
    setWindowFilePath("Untitled  - Mapox2D");

    createActions();
    createMapView();
    createMapScene();
    createStatusBar();
    createUndoView();
    createDockWindows();

    connect(m_textureList, &QListWidget::itemClicked, m_mapScene, &MapScene::currentTextureSelectedInList);
    connect(m_undoStack, &QUndoStack::indexChanged, this, &MainWindow::docWasModified);
    connect(m_textureList, &TextureList::docModified, this, &MainWindow::docWasModified);
    connect(m_mapScene, &MapScene::triggerTool, m_mapView, &MapView::toolTriggered);

    connect(this, &MainWindow::toolTriggered, m_mapView, &MapView::toolTriggered);
    connect(m_mapView, &MapView::checkTool, this, &MainWindow::checkedTool);

    QPalette pal = palette();
    pal.setColor(QPalette::Window,     QColor{ 60,  60, 60 }); // general background
    pal.setColor(QPalette::WindowText, QColor{210, 210, 210});
    pal.setColor(QPalette::Base,       QColor{ 60,  60, 60 }); // some backgrounds
    pal.setColor(QPalette::Text,       QColor{210, 210, 210});
    setPalette(pal);
    setAutoFillBackground(true);

    menuBar()->setStyleSheet("QMenuBar{background-color:rgb(60, 60, 60); color:rgb(210, 210, 210)}");
    m_textureList->setStyleSheet("QListView{"
                                 "    background          :rgb(60, 60, 60);"
                                 "    border              :1px solid rgb(100, 100, 100);"
                                 "}"
                                 ""
                                 ""
                                 ""
                                 "QWidget{"
                                 "    background          : rgb(60, 60, 60);"
                                 "}"
                                 ""
                                 ""
                                 ""
                                 "QScrollBar:horizontal{"
                                 "    background          : rgb(60, 60 ,60);"
                                 "    border              : 1px solid rgb(100, 100, 100);"
                                 "    height              : 19px;"
                                 "    margin              : 0px 20px 0px 20px;"
                                 "}"
                                 "QScrollBar::handle:horizontal{"
                                 "    background          : darkgray;"
                                 "    min-width           : 20px;"
                                 "}"
                                 "QScrollBar::handle:horizontal:hover{"
                                 "    background          : rgb(150, 150, 150);"
                                 "}"
                                 "QScrollBar::add-line:horizontal{"
                                 "    background          : darkgray;"
                                 "    width               : 19px;"
                                 "    border              : 1px solid rgb(100, 100, 100);"
                                 "    border-right        : 1px solid rgb( 60,  60,  60);"
                                 "    subcontrol-position : right;"
                                 "    subcontrol-origin   : margin;"
                                 "}"
                                 "QScrollBar::sub-line:horizontal{"
                                 "    background          : darkgray;"
                                 "    width               : 19px;"
                                 "    border              : 1px solid rgb(100, 100, 100);"
                                 "    border-right        : 1px solid rgb( 60,  60,  60);"
                                 "    subcontrol-position : left;"
                                 "    subcontrol-origin   : margin;"
                                 "}"
                                 "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal{"
                                 "    width               : 3px;"
                                 "    height              : 3px;"
                                 "    background          : rgb(60, 60, 60);"
                                 "}"
                                 "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal{"
                                 "    background          : rgb(60, 60, 60);"
                                 "}"
                                 ""
                                 ""
                                 ""
                                 ""
                                 "QScrollBar:vertical{"
                                 "    background          : rgb(60, 60 ,60);"
                                 "    border              : 1px solid rgb(100, 100, 100);"
                                 "    width               : 19px;"
                                 "    margin              : 20px 0px 20px 0px;"
                                 "}"
                                 "QScrollBar::handle:vertical{"
                                 "    background          : darkgray;"
                                 "    min-height          : 20px;"
                                 "}"
                                 "QScrollBar::handle:vertical:hover{"
                                 "    background          : rgb(150, 150, 150);"
                                 "}"
                                 "QScrollBar::add-line:vertical{"
                                 "    background          : darkgray;"
                                 "    height              : 19px;"
                                 "    border              : 1px solid rgb(100, 100, 100);"
                                 "    border-bottom       : 1px solid rgb( 60,  60,  60);"
                                 "    subcontrol-position : bottom;"
                                 "    subcontrol-origin   : margin;"
                                 "}"
                                 "QScrollBar::sub-line:vertical{"
                                 "    background          : darkgray;"
                                 "    height              : 19px;"
                                 "    border              : 1px solid rgb(100, 100, 100);"
                                 "    border-top          : 1px solid rgb( 60,  60,  60);"
                                 "    subcontrol-position : top;"
                                 "    subcontrol-origin   : margin;"
                                 "}"
                                 "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical{"
                                 "    width               : 3px;"
                                 "    height              : 3px;"
                                 "    background          : rgb(60, 60, 60);"
                                 "}"
                                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{"
                                 "    background          : rgb(60, 60, 60);"
                                 "}");
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
        event->accept();
    else
        event->ignore();
}

void MainWindow::newMap()
{
    if(maybeSave()){
        NewMapDialog dial{this};
        auto values = dial.processDial();
        if(!values.empty()){
            m_undoStack->clear();
            m_textureList->clean();
            setCurrentFile(QString{});
            createMapScene(values);
        }
    }
}

void MainWindow::open()
{
    if(maybeSave()){
        const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                              tr("M2X Files (*.m2x)"));
        if(!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if(m_currentFile.isEmpty())
        return saveAs();
    return saveFile(m_currentFile);
}

bool MainWindow::saveAs()
{
    QFileDialog dial{this};
    dial.setWindowModality(Qt::WindowModal);
    dial.setAcceptMode(QFileDialog::AcceptSave);
    dial.setDefaultSuffix("m2x");
    dial.setNameFilter(tr("Map (*.m2x)"));
    dial.selectFile("untitled");
    if(dial.exec() != QDialog::Accepted)
        return false;
    return saveFile(dial.selectedFiles().first());
}

void MainWindow::docWasModified()
{
    qDebug() << "MainWindow::docWasModified - Document modified";
    m_documentModified = true;
}

void MainWindow::about()
{
    QMessageBox message{};
    message.setIcon(QMessageBox::Information);
    message.setText("Mapox2D was created in 2019");
    message.exec();
}

void MainWindow::openTexture()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(
                                      this, tr("Select one or more textures to open"), "",
                                      tr("Images (*.png *.jpg *.bmp)"));
    for(const auto& name : fileNames){
        if(!name.isEmpty())
            m_textureList->addTexture(name);
    }
}

void MainWindow::openSpriteSheet()
{
    const QString fileName = QFileDialog::getOpenFileName(
                                      this, tr("Select Sprite Sheet to open"), "",
                                      tr("Images (*.png *.jpg *.bmp)"));
    const QPixmap spriteSheet{fileName};

    if(!spriteSheet.isNull()){
        ImportSpriteSheetDialog dial{spriteSheet, this};
        const QList<QPixmap> result = dial.processDial();
        m_textureList->addTexture(result, fileName);
    }
}

void MainWindow::quit()
{
    if(maybeSave())
        qApp->quit();
}

void MainWindow::selectToolTriggered(bool trigger)
{
    emit toolTriggered(trigger, ToolType::Selection);
}

void MainWindow::moveSelectionToolTriggered(bool trigger)
{
    emit toolTriggered(trigger, ToolType::MoveSelection);
}

void MainWindow::brushTileToolTriggered(bool trigger)
{
    emit toolTriggered(trigger, ToolType::Brush);
}

void MainWindow::checkedTool(ToolType type)
{
    m_selectAct->setChecked(false);
    m_moveSelectionAct->setChecked(false);
    m_brushTileAct->setChecked(false);

    switch(type){
    case ToolType::Selection :
        m_selectAct->setChecked(true);
        break;
    case ToolType::MoveSelection :
        m_moveSelectionAct->setChecked(true);
        break;
    case ToolType::Brush :
        m_brushTileAct->setChecked(true);
        break;
    case ToolType::NoTool :
        break;
    }
}

void MainWindow::replaceTextureList(bool replace)
{
    if(replace){
        m_dockTextureList->setFloating(false);
    }
}

void MainWindow::replaceUndoView(bool replace)
{
    if(replace){
        m_dockUndoView->setFloating(false);
    }
}

void MainWindow::createActions()
{
    const QString toolBarStyle{"QToolBar{"
                               "    border:2px ridge rgb(60, 60, 60);"
                               "}"
                               "QToolBar::separator{"
                               "    background-color:rgb(80, 80, 80);"
                               "    width:1px;"
                               "}"
                               "QToolBar::handle{"
                               "    background-color:rgb(100, 100, 100);"
                               "    width:2px;"
                               "}"};

    /////////////////////// FILE MENU
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->setStyleSheet(toolBarStyle);

    // NEW FILE
    QIcon newIcon{":/icons/newFile.png"};
    QAction *newAct = new QAction{newIcon, tr("&New map"), this};
    newAct->setShortcut(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::newMap);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    // OPEN FILE
    QIcon openIcon{":/icons/openFile.png"};
    QAction *openAct = new QAction{openIcon, tr("&Open..."), this};
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // SAVE FILE
    QIcon saveIcon{":/icons/saveFile.png"};
    QAction *saveAct = new QAction{saveIcon, tr("&Save"), this};
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    // SAVE AS
    QAction *saveAsAct = new QAction{tr("&Save as..."), this};
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileToolBar->addSeparator();

    // IMPORT TEXTURE
    QIcon importTextureIcon{":/icons/importTexture.png"};
    QAction *importTextureAct = new QAction{importTextureIcon, tr("&Import texture..."), this};
    connect(importTextureAct, &QAction::triggered, this, &MainWindow::openTexture);
    fileMenu->addAction(importTextureAct);
    fileToolBar->addAction(importTextureAct);

    // IMPORT SPRITESHEET
    QIcon importSpriteSheetIcon{":/icons/importSpriteSheet.png"};
    QAction *importSpriteSheetAct = new QAction{importSpriteSheetIcon, tr("Import sprite sheet..."), this};
    connect(importSpriteSheetAct, &QAction::triggered, this, &MainWindow::openSpriteSheet);
    fileMenu->addAction(importSpriteSheetAct);
    fileToolBar->addAction(importSpriteSheetAct);

    fileMenu->addSeparator();

    // QUIT
    QAction *quitAct = new QAction{tr("&Quit"), this};
    quitAct->setShortcut(QKeySequence{"Ctrl+Q"});
    connect(quitAct, &QAction::triggered, this, &MainWindow::quit);
    fileMenu->addAction(quitAct);

    /////////////////////// EDIT MENU
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar("Edit");
    editToolBar->setStyleSheet(toolBarStyle);

    // UNDO
    QIcon undoIcon{":/icons/undo.png"};
    QAction *undoAct = m_undoStack->createUndoAction(this, tr("&Undo"));
    undoAct->setIcon(undoIcon);
    undoAct->setShortcut(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, m_mapView, &MapView::undoActTriggered);
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    // REDO
    QIcon redoIcon{":/icons/redo.png"};
    QAction *redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    redoAct->setIcon(redoIcon);
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);

    editMenu->addSeparator();

    // COPY
    QAction *copyAct = new QAction{tr("Copy Selection"), this};
    copyAct->setShortcut(QKeySequence{"Ctrl+C"});
    connect(copyAct, &QAction::triggered, m_mapView, &MapView::copyTriggered);
    editMenu->addAction(copyAct);

    // CUT
    QAction *cutAct = new QAction{tr("Cut Selection"), this};
    cutAct->setShortcut(QKeySequence{"Ctrl+X"});
    connect(cutAct, &QAction::triggered, m_mapView, &MapView::cutTriggered);
    editMenu->addAction(cutAct);

    // PASTE
    QAction *pasteAct = new QAction{tr("Paste Selection"), this};
    pasteAct->setShortcut(QKeySequence{"Ctrl+V"});
    connect(pasteAct, &QAction::triggered, m_mapView, &MapView::pasteTriggered);
    editMenu->addAction(pasteAct);

    editMenu->addSeparator();

    /////////////////////// TOOL MENU
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = addToolBar("Tools");
    toolsToolBar->setStyleSheet(toolBarStyle);

    // SELECTION
    QIcon selectIcon{":/icons/selection.png"};
    m_selectAct = new QAction{selectIcon, tr("&Selection Tool"), this};
    m_selectAct->setCheckable(true);
    connect(m_selectAct, &QAction::triggered, this, &MainWindow::selectToolTriggered);
    toolsMenu->addAction(m_selectAct);
    toolsToolBar->addAction(m_selectAct);

    // MOVE SELECTION
    QIcon moveSelectionIcon{":/icons/moveSelection.png"};
    m_moveSelectionAct = new QAction{moveSelectionIcon, tr("&Move Selection Tool"), this};
    m_moveSelectionAct->setCheckable(true);
    connect(m_moveSelectionAct, &QAction::triggered, this, &MainWindow::moveSelectionToolTriggered);
    toolsMenu->addAction(m_moveSelectionAct);
    toolsToolBar->addAction(m_moveSelectionAct);

    // BRUSH TILE
    QIcon brushTileIcon{":/icons/brushTile.png"};
    m_brushTileAct = new QAction{brushTileIcon, tr("&Brush Tile Tool"), this};
    m_brushTileAct->setCheckable(true);
    connect(m_brushTileAct, &QAction::triggered, this, &MainWindow::brushTileToolTriggered);
    toolsMenu->addAction(m_brushTileAct);
    toolsMenu->addSeparator();
    toolsToolBar->addAction(m_brushTileAct);
    toolsToolBar->addSeparator();

    // FILL SELECTION (OR ALL MAP IF NO SELECTION)
    QIcon fillSelectionIcon{":/icons/fill.png"};
    QAction *fillSelectionAct = new QAction{fillSelectionIcon, tr("F&ill with current texture"), this};
    fillSelectionAct->setShortcut(QKeySequence{tr("Ctrl+,")});
    connect(fillSelectionAct, &QAction::triggered, m_mapView, &MapView::fillSelection);
    toolsMenu->addAction(fillSelectionAct);
    toolsToolBar->addAction(fillSelectionAct);

    // ANCHOR PASTED SELECTION
    QIcon anchorIcon{":/icons/anchor.png"};
    QAction *anchorAct = new QAction{anchorIcon, tr("Anc&hor pasted selection"), this};
    anchorAct->setDisabled(true);
    connect(anchorAct, &QAction::triggered, m_mapView, &MapView::anchorSelection);
    connect(m_mapView, &MapView::activeAnchorAct, anchorAct, &QAction::setEnabled);
    toolsMenu->addAction(anchorAct);
    toolsToolBar->addAction(anchorAct);

    /////////////////////// HELP MENU
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    // ABOUT
    QAction *aboutAct = new QAction{tr("&About"), this};
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAct);
}

void MainWindow::createMapView()
{
    m_mapView->setScene(m_mapScene);
    m_mapView->setMinimumWidth(640);
    m_mapView->setMinimumHeight(360);
    m_mapView->setBackgroundBrush(QBrush{QColor{100, 100, 100}});
    m_mapView->setCacheMode(QGraphicsView::CacheBackground);
    m_mapView->holdStatusBar(statusBar());
    m_mapView->holdUndoStack(m_undoStack);
    m_mapView->setStyleSheet("QGraphicsView{"
                             "    border              : rgb(100, 100, 100);"
                             "}"
                             ""
                             ""
                             ""
                             "QWidget{"
                             "    background          : rgb(60, 60, 60);"
                             "}"
                             ""
                             ""
                             ""
                             "QScrollBar:horizontal{"
                             "    background          : rgb(60, 60 ,60);"
                             "    border              : 1px solid rgb(100, 100, 100);"
                             "    height              : 19px;"
                             "    margin              : 0px 20px 0px 20px;"
                             "}"
                             "QScrollBar::handle:horizontal{"
                             "    background          : darkgray;"
                             "    min-width           : 20px;"
                             "}"
                             "QScrollBar::handle:horizontal:hover{"
                             "    background          : rgb(150, 150, 150);"
                             "}"
                             "QScrollBar::add-line:horizontal{"
                             "    background          : darkgray;"
                             "    width               : 19px;"
                             "    border              : 1px solid rgb(100, 100, 100);"
                             "    border-right        : 1px solid rgb( 60,  60,  60);"
                             "    subcontrol-position : right;"
                             "    subcontrol-origin   : margin;"
                             "}"
                             "QScrollBar::sub-line:horizontal{"
                             "    background          : darkgray;"
                             "    width               : 19px;"
                             "    border              : 1px solid rgb(100, 100, 100);"
                             "    border-right        : 1px solid rgb( 60,  60,  60);"
                             "    subcontrol-position : left;"
                             "    subcontrol-origin   : margin;"
                             "}"
                             "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal{"
                             "    width               : 3px;"
                             "    height              : 3px;"
                             "    background          : rgb(60, 60, 60);"
                             "}"
                             "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal{"
                             "    background          : rgb(60, 60, 60);"
                             "}"
                             ""
                             ""
                             ""
                             ""
                             "QScrollBar:vertical{"
                             "    background          : rgb(60, 60 ,60);"
                             "    border              : 1px solid rgb(100, 100, 100);"
                             "    width               : 19px;"
                             "    margin              : 20px 0px 20px 0px;"
                             "}"
                             "QScrollBar::handle:vertical{"
                             "    background          : darkgray;"
                             "    min-height          : 20px;"
                             "}"
                             "QScrollBar::handle:vertical:hover{"
                             "    background          : rgb(150, 150, 150);"
                             "}"
                             "QScrollBar::add-line:vertical{"
                             "    background          : darkgray;"
                             "    height              : 19px;"
                             "    border              : 1px solid rgb(100, 100, 100);"
                             "    border-bottom       : 1px solid rgb( 60,  60,  60);"
                             "    subcontrol-position : bottom;"
                             "    subcontrol-origin   : margin;"
                             "}"
                             "QScrollBar::sub-line:vertical{"
                             "    background          : darkgray;"
                             "    height              : 19px;"
                             "    border              : 1px solid rgb(100, 100, 100);"
                             "    border-top          : 1px solid rgb( 60,  60,  60);"
                             "    subcontrol-position : top;"
                             "    subcontrol-origin   : margin;"
                             "}"
                             "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical{"
                             "    width               : 3px;"
                             "    height              : 3px;"
                             "    background          : rgb(60, 60, 60);"
                             "}"
                             "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{"
                             "    background          : rgb(60, 60, 60);"
                             "}");
}

void MainWindow::createMapScene()
{
    m_mapScene->holdStatusBar(statusBar());
    m_mapScene->holdTextureList(m_textureList);

    connect(m_mapScene, &MapScene::mouseMoveAndPressLeft,
            m_mapView, &MapView::mouseMovingAndPressing);
    connect(m_mapScene, &MapScene::itemFocusChange,
            m_mapView, &MapView::itemFocusChanged);

    std::map<NewMapDialog::FieldType, int> defaultMap{};
    defaultMap[NewMapDialog::FieldType::Width]  = 32;
    defaultMap[NewMapDialog::FieldType::Height] = 32;
    defaultMap[NewMapDialog::FieldType::Rows]   = 6;
    defaultMap[NewMapDialog::FieldType::Cols]   = 6;
    createMapScene(defaultMap);
}

void MainWindow::createMapScene(std::map<NewMapDialog::FieldType, int>& values)
{
    m_documentModified = false;
    m_mapView->reset();
    m_mapScene->createMatrix(values[NewMapDialog::FieldType::Width],
                             values[NewMapDialog::FieldType::Height],
                             values[NewMapDialog::FieldType::Rows],
                             values[NewMapDialog::FieldType::Cols]);
    m_mapView->reset(m_mapScene->itemsBoundingRect());
}

void MainWindow::createStatusBar()
{
    // Status Bar is automatically created the first time statusBar() is called
    // This function return a pointer to the main window's qstatusbar
    statusBar()->showMessage(tr("Ready !"));
}

void MainWindow::createUndoView()
{
    m_undoView = new QUndoView{m_undoStack};
    m_undoView->setWindowTitle(tr("Command List"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);
    m_undoView->setStyleSheet("QListView{"
                              "    background : rgb(60, 60, 60);"
                              "    border     : 1px solid rgb(100, 100, 100);"
                              "    color      : rgb(210, 210, 210);"
                              "}"
                              ""
                              ""
                              ""
                              "QWidget{"
                              "    background          : rgb(60, 60, 60);"
                              "}"
                              ""
                              ""
                              ""
                              "QScrollBar:horizontal{"
                              "    background          : rgb(60, 60 ,60);"
                              "    border              : 1px solid rgb(100, 100, 100);"
                              "    height              : 19px;"
                              "    margin              : 0px 20px 0px 20px;"
                              "}"
                              "QScrollBar::handle:horizontal{"
                              "    background          : darkgray;"
                              "    min-width           : 20px;"
                              "}"
                              "QScrollBar::handle:horizontal:hover{"
                              "    background          : rgb(150, 150, 150);"
                              "}"
                              "QScrollBar::add-line:horizontal{"
                              "    background          : darkgray;"
                              "    width               : 19px;"
                              "    border              : 1px solid rgb(100, 100, 100);"
                              "    border-right        : 1px solid rgb( 60,  60,  60);"
                              "    subcontrol-position : right;"
                              "    subcontrol-origin   : margin;"
                              "}"
                              "QScrollBar::sub-line:horizontal{"
                              "    background          : darkgray;"
                              "    width               : 19px;"
                              "    border              : 1px solid rgb(100, 100, 100);"
                              "    border-right        : 1px solid rgb( 60,  60,  60);"
                              "    subcontrol-position : left;"
                              "    subcontrol-origin   : margin;"
                              "}"
                              "QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal{"
                              "    width               : 3px;"
                              "    height              : 3px;"
                              "    background          : rgb(60, 60, 60);"
                              "}"
                              "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal{"
                              "    background          : rgb(60, 60, 60);"
                              "}"
                              ""
                              ""
                              ""
                              ""
                              "QScrollBar:vertical{"
                              "    background          : rgb(60, 60 ,60);"
                              "    border              : 1px solid rgb(100, 100, 100);"
                              "    width               : 19px;"
                              "    margin              : 20px 0px 20px 0px;"
                              "}"
                              "QScrollBar::handle:vertical{"
                              "    background          : darkgray;"
                              "    min-height          : 20px;"
                              "}"
                              "QScrollBar::handle:vertical:hover{"
                              "    background          : rgb(150, 150, 150);"
                              "}"
                              "QScrollBar::add-line:vertical{"
                              "    background          : darkgray;"
                              "    height              : 19px;"
                              "    border              : 1px solid rgb(100, 100, 100);"
                              "    border-bottom       : 1px solid rgb( 60,  60,  60);"
                              "    subcontrol-position : bottom;"
                              "    subcontrol-origin   : margin;"
                              "}"
                              "QScrollBar::sub-line:vertical{"
                              "    background          : darkgray;"
                              "    height              : 19px;"
                              "    border              : 1px solid rgb(100, 100, 100);"
                              "    border-top          : 1px solid rgb( 60,  60,  60);"
                              "    subcontrol-position : top;"
                              "    subcontrol-origin   : margin;"
                              "}"
                              "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical{"
                              "    width               : 3px;"
                              "    height              : 3px;"
                              "    background          : rgb(60, 60, 60);"
                              "}"
                              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical{"
                              "    background          : rgb(60, 60, 60);"
                              "}");
}

void MainWindow::createDockWindows()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    const QString styleSheet{"QDockWidget::close-button, QDockWidget::float-button{"
                             "    background    : darkgray;"
                             "    border        : 1px solid rgb(100, 100, 100);"
                             "}"
                             "QDockWidget::close-button:hover{"
                             "    background    : rgb(220, 40, 40);"
                             "}"
                             "QDockWidget::float-button:hover{"
                             "    background    : rgb(50, 130, 150);"
                             "}"
                             "QDockWidget::title{"
                             "    text-align    : left;"
                             "    color         : rgb(210, 210, 210);"
                             "    background    : rgb(60, 60 ,60);"
                             "    border        : 1px solid rgb(100, 100, 100);"
                             "    border-bottom : none;"
                             "}"
                             "QDockWidget{"
                             "    color         : rgb(210, 210, 210);"
                             "    border        : 1px solid rgb(100, 100, 100);"
                             "}"};

    m_dockTextureList = new QDockWidget{tr("Texture List"), this};
    m_dockTextureList->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockTextureList->setWidget(m_textureList);
    m_dockTextureList->setMinimumWidth(200);
    m_dockTextureList->setStyleSheet(styleSheet);
    addDockWidget(Qt::RightDockWidgetArea, m_dockTextureList);
    QAction *viewTextureListAct = m_dockTextureList->toggleViewAction();
    m_viewMenu->addAction(viewTextureListAct);
    connect(viewTextureListAct, &QAction::triggered, this, &MainWindow::replaceTextureList);

    m_dockUndoView = new QDockWidget{tr("Command List"), this};
    m_dockUndoView->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockUndoView->setWidget(m_undoView);
    m_dockUndoView->setMinimumWidth(200);
    m_dockUndoView->setMaximumWidth(300);
    m_dockUndoView->setStyleSheet(styleSheet);
    m_dockUndoView->setFloating(false);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockUndoView);
    QAction *viewCommandListAct = m_dockUndoView->toggleViewAction();
    m_viewMenu->addAction(viewCommandListAct);
    connect(viewCommandListAct, &QAction::triggered, this, &MainWindow::replaceUndoView);
}

bool MainWindow::maybeSave()
{
    if(!m_documentModified)
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{ 
    DataSaver ds{m_mapScene, m_textureList};
    if(!ds.saveAllDatas(fileName))
        return false;

    setCurrentFile(fileName);
    statusBar()->showMessage("Map saved", 3000);
    m_documentModified = false;

    return true;
}

void MainWindow::setCurrentFile(const QString& fileName)
{
    m_currentFile = fileName;
    QString showName = StringTools::cutExtensionFileName(m_currentFile);
    if(m_currentFile.isEmpty())
        showName = "Untitled";
    setWindowFilePath(showName + " - Mapox2D");
}

void MainWindow::loadFile(const QString& fileName)
{
    DataLoader dl{m_textureList, m_mapScene};
    dl.loadData(fileName);

    m_mapView->reset(m_mapScene->itemsBoundingRect());
    m_undoStack->clear();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Map loaded"), 3000);

    m_documentModified = false;
}
