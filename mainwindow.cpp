#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      m_centralWidget{new QWidget{this}},
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
        // NewMapDialog::processDial return a map<Qstring, int> that contains values :
        // ["tileWidth"]  => val(int)
        // ["tileHeight"] => val(int)
        // ["totalRows"]  => val(int)
        // ["totalCols"]  => val(int)
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
    qDebug() << "Document modified";
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
                                      this, tr("Select one or more files to open"), "",
                                      tr("Images (*.png *.jpg *.bmp)"));
    for(const auto& name : fileNames){
        if(!name.isEmpty())
            m_textureList->addTexture(name);
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

void MainWindow::createActions()
{
    /////////////////////// FILE MENU
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    // NEW FILE
    QIcon newIcon{":/icons/newFile.png"};
    QAction *newAct = new QAction{newIcon, tr("&New map"), this};
    newAct->setShortcut(QKeySequence::New);
    //newAct->setStatusTip(tr("Create a new map"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newMap);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    // OPEN FILE
    QIcon openIcon{":/icons/openFile.png"};
    QAction *openAct = new QAction{openIcon, tr("&Open..."), this};
    openAct->setShortcut(QKeySequence::Open);
    //openAct->setStatusTip(tr("Open existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // SAVE FILE
    QIcon saveIcon{":/icons/saveFile.png"};
    QAction *saveAct = new QAction{saveIcon, tr("&Save"), this};
    saveAct->setShortcut(QKeySequence::Save);
    //saveAct->setStatusTip(tr("Save current file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    // SAVE AS
    QAction *saveAsAct = new QAction{tr("&Save as..."), this};
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    //saveAsAct->setStatusTip(tr("Save current file as..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileToolBar->addSeparator();

    // IMPORT TEXTURE
    QIcon importTextureIcon{":/icons/importTexture.png"};
    QAction *importTextureAct = new QAction{importTextureIcon, tr("&Import texture..."), this};
    //importTextureAct->setStatusTip(tr("Import texture from computer"));
    connect(importTextureAct, &QAction::triggered, this, &MainWindow::openTexture);
    fileMenu->addAction(importTextureAct);
    fileToolBar->addAction(importTextureAct);

    fileMenu->addSeparator();

    // QUIT
    QAction *quitAct = new QAction{tr("&Quit"), this};
    quitAct->setShortcut(QKeySequence{"Ctrl+Q"});
    //quitAct->setStatusTip(tr("Shutdown Mapox2D"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::quit);
    fileMenu->addAction(quitAct);

    /////////////////////// EDIT MENU
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar("Edit");

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
    editToolBar->addSeparator();

    editMenu->addSeparator();

    // COPY
    QAction *copyAct = new QAction{tr("Copy"), this};
    copyAct->setShortcut(QKeySequence{"Ctrl+C"});
    //copyAct->setStatusTip(tr("Copy the current selection"));
    connect(copyAct, &QAction::triggered, m_mapView, &MapView::copyTriggered);
    editMenu->addAction(copyAct);

    // PASTE
    QAction *pasteAct = new QAction{tr("Paste"), this};
    pasteAct->setShortcut(QKeySequence{"Ctrl+V"});
    //pasteAct->setStatusTip(tr("Paste from clipboard"));
    connect(pasteAct, &QAction::triggered, m_mapView, &MapView::pasteTriggered);
    editMenu->addAction(pasteAct);

    editMenu->addSeparator();

    /////////////////////// TOOL MENU
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = addToolBar("Tools");

    // SELECTION
    QIcon selectIcon{":/icons/selection.png"};
    m_selectAct = new QAction{selectIcon, tr("&Selection Tool"), this};
    //m_selectAct->setStatusTip(tr("Rectangular selection"));
    m_selectAct->setCheckable(true);
    connect(m_selectAct, &QAction::triggered, this, &MainWindow::selectToolTriggered);
    toolsMenu->addAction(m_selectAct);
    toolsToolBar->addAction(m_selectAct);

    // MOVE SELECTION
    QIcon moveSelectionIcon{":/icons/moveSelection.png"};
    m_moveSelectionAct = new QAction{moveSelectionIcon, tr("&Move Selection Tool"), this};
    //m_moveSelectionAct->setStatusTip(tr("Move current selection"));
    m_moveSelectionAct->setCheckable(true);
    connect(m_moveSelectionAct, &QAction::triggered, this, &MainWindow::moveSelectionToolTriggered);
    toolsMenu->addAction(m_moveSelectionAct);
    toolsToolBar->addAction(m_moveSelectionAct);

    // BRUSH TILE
    QIcon brushTileIcon{":/icons/brushTile.png"};
    m_brushTileAct = new QAction{brushTileIcon, tr("&Brush Tile Tool"), this};
    //m_brushTileAct->setStatusTip(tr("Fill a tile with texture selected in list"));
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
    //fillSelectionAct->setStatusTip(tr("Fill selected region or all map if no selection"));
    connect(fillSelectionAct, &QAction::triggered, m_mapView, &MapView::fillSelection);
    toolsMenu->addAction(fillSelectionAct);
    toolsToolBar->addAction(fillSelectionAct);

    // ANCHOR PASTED SELECTION
    QIcon anchorIcon{":/icons/anchor.png"};
    QAction *anchorAct = new QAction{anchorIcon, tr("Anc&hor pasted selection"), this};
    //anchorAct->setStatusTip(tr("Anchor the selection previously pasted"));
    anchorAct->setDisabled(true);
    connect(anchorAct, &QAction::triggered, m_mapView, &MapView::anchorSelection);
    connect(m_mapView, &MapView::activeAnchorAct, anchorAct, &QAction::setEnabled);
    toolsMenu->addAction(anchorAct);
    toolsToolBar->addAction(anchorAct);

    /////////////////////// HELP MENU
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    // ABOUT
    QAction *aboutAct = new QAction{tr("&About"), this};
    //aboutAct->setStatusTip(tr("About this software"));
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
}

void MainWindow::createMapScene()
{
    m_mapScene->holdStatusBar(statusBar());
    m_mapScene->holdTextureList(m_textureList);

    connect(m_mapScene, &MapScene::mouseMoveAndPressLeft,
            m_mapView, &MapView::mouseMovingAndPressing);
    connect(m_mapScene, &MapScene::itemFocusChange,
            m_mapView, &MapView::itemFocusChanged);

    std::map<QString, int> defaultMap{};
    defaultMap["tileWidth"]  = 32;
    defaultMap["tileHeight"] = 32;
    defaultMap["totalRows"]  = 6;
    defaultMap["totalCols"]  = 6;
    createMapScene(defaultMap);
}

void MainWindow::createMapScene(std::map<QString, int>& values)
{
    m_documentModified = false;
    m_mapView->reset();
    m_mapScene->createMatrix(values["tileWidth"],
                             values["tileHeight"],
                             values["totalRows"],
                             values["totalCols"]);
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
}

void MainWindow::createDockWindows()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    const int minWidth{200};
    const int maxWidth{300};

    QDockWidget *dockTextureList = new QDockWidget{tr("Texture List"), this};
    dockTextureList->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockTextureList->setWidget(m_textureList);
    dockTextureList->setMinimumWidth(minWidth);
    dockTextureList->setMaximumWidth(maxWidth);
    addDockWidget(Qt::RightDockWidgetArea, dockTextureList);
    m_viewMenu->addAction(dockTextureList->toggleViewAction());

    QDockWidget *dockUndoView = new QDockWidget{tr("Command List"), this};
    dockUndoView->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockUndoView->setWidget(m_undoView);
    dockUndoView->setMinimumWidth(minWidth);
    dockUndoView->setMaximumWidth(maxWidth);
    addDockWidget(Qt::LeftDockWidgetArea, dockUndoView);
    m_viewMenu->addAction(dockUndoView->toggleViewAction());
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
