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
    setMinimumWidth(1080);
    registerStyleSheet();

    createActions();
    createMapView();
    createMapScene();
    createStatusBar();
    createUndoView();
    createDockWindows();

    connect(m_textureList , &QListWidget::itemClicked,
            m_mapScene    , &MapScene::currentTextureSelectedInList);
    connect(m_undoStack   , &QUndoStack::indexChanged,
            this          , &MainWindow::docWasModified);
    connect(m_textureList , &TextureList::docModified,
            this          , &MainWindow::docWasModified);
    connect(m_textureList , &TextureList::removeTileFromScene,
            m_mapView     , &MapView::removeTileFromScene);
    connect(m_textureList , &TextureList::removeItemFromTextureList,
            this          , &MainWindow::removeItemFromTextureList);
    connect(m_mapScene    , &MapScene::triggerTool,
            m_mapView     , &MapView::toolTriggered);

    setStyleSheet("QMainWindow{background:rgb(60, 60, 60);}");
    m_textureList->setStyleSheet(m_styleRegister[StyleSheet::TextureList]);
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
    message.setText("Mapox2D - In development since January 10 2019");
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

void MainWindow::generateDatas()
{
    const QString userGeneratePath =
            QFileDialog::getExistingDirectory(this, tr("Select folder to generate datas"));
    DataGenerator dg{m_mapScene};
    dg.generateDatas(userGeneratePath);
    QMessageBox::information(this,
                             tr("Generated Files"),
                             tr("Files generated successefuly !"));
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

void MainWindow::clearUndoStack()
{
    m_undoStack->clear();
}

void MainWindow::removeItemFromTextureList(const QString& textureName)
{
    if(!textureName.isEmpty()){
        RemoveItemFromTextureListCommand *cmd =
                new RemoveItemFromTextureListCommand{m_textureList, textureName};
        m_undoStack->push(cmd);
    }
}

void MainWindow::changeBackgroundViewColor()
{
    QColorDialog dial{};
    dial.exec();
    if(dial.result() == QDialog::Accepted){
        const QColor selected = dial.selectedColor();
        m_mapView->setBackgroundBrush(QBrush{selected});
    }
}

void MainWindow::changeGridColor()
{
    QColorDialog dial{};
    dial.exec();
    if(dial.result() == QDialog::Accepted){
        const QColor selected = dial.selectedColor();
        m_mapScene->changeGridColor(selected);
    }
}

void MainWindow::createActions()
{
    const QString toolBarStyle{m_styleRegister[StyleSheet::ToolBar]};

    /////////////////////// FILE MENU
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    menuBar()->setStyleSheet(m_styleRegister[StyleSheet::MenuBar]);
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->setStyleSheet(toolBarStyle);

    // NEW FILE
    QIcon newIcon{":/icons/newFile.png"};
    QAction *newAct = new QAction{newIcon, tr("&New project"), this};
    newAct->setShortcut(QKeySequence::New);
    newAct->setToolTip(tr("New project"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newMap);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    // OPEN FILE
    QIcon openIcon{":/icons/openFile.png"};
    QAction *openAct = new QAction{openIcon, tr("&Open..."), this};
    openAct->setShortcut(QKeySequence::Open);
    openAct->setToolTip(tr("Open project"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    // SAVE FILE
    QIcon saveIcon{":/icons/saveFile.png"};
    QAction *saveAct = new QAction{saveIcon, tr("&Save"), this};
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setToolTip(tr("Save project"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    // SAVE AS
    QAction *saveAsAct = new QAction{tr("&Save as..."), this};
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setToolTip(tr("Save project as..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileToolBar->addSeparator();
    fileMenu->addSeparator();

    // IMPORT TEXTURE
    QIcon importTextureIcon{":/icons/importTexture.png"};
    QAction *importTextureAct = new QAction{importTextureIcon, tr("&Import texture..."), this};
    importTextureAct->setToolTip(tr("Import texture from computer"));
    connect(importTextureAct, &QAction::triggered, this, &MainWindow::openTexture);
    fileMenu->addAction(importTextureAct);
    fileToolBar->addAction(importTextureAct);

    // IMPORT SPRITESHEET
    QIcon importSpriteSheetIcon{":/icons/importSpriteSheet.png"};
    QAction *importSpriteSheetAct = new QAction{importSpriteSheetIcon, tr("Import sprite sheet..."), this};
    importSpriteSheetAct->setToolTip(tr("Import sprite (After the import, cut the sheet is asked)"));
    connect(importSpriteSheetAct, &QAction::triggered, this, &MainWindow::openSpriteSheet);
    fileMenu->addAction(importSpriteSheetAct);
    fileToolBar->addAction(importSpriteSheetAct);

    // GENERATE FILES
    QIcon generateIcon{":/icons/generate.png"};
    QAction *generateAct = new QAction{generateIcon, tr("Generate Datas...")};
    generateAct->setToolTip(tr("Generate folder with datas that represent textures and array"));
    connect(generateAct, &QAction::triggered, this, &MainWindow::generateDatas);
    fileMenu->addAction(generateAct);
    fileToolBar->addAction(generateAct);

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
    undoAct->setToolTip(tr("Undo"));
    undoAct->setShortcut(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, m_mapView, &MapView::undoActTriggered);
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    // REDO
    QIcon redoIcon{":/icons/redo.png"};
    QAction *redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    redoAct->setIcon(redoIcon);
    redoAct->setToolTip(tr("Redo"));
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);

    editMenu->addSeparator();
    editToolBar->addSeparator();

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

    // RESIZE GRID
    QIcon resizeGridIcon{":/icons/resizeGrid.png"};
    QAction *resizeGridAct = new QAction{resizeGridIcon, tr("Resize grid")};
    resizeGridAct->setToolTip(tr("Resize the grid with given rows and cols"));
    connect(resizeGridAct, &QAction::triggered, m_mapView, &MapView::resizeGridTriggered);
    editMenu->addAction(resizeGridAct);
    editToolBar->addAction(resizeGridAct);

    // CHANGE BACKGROUND COLOR
    QIcon changeBackgroundColorIcon{":/icons/changeBackgroundColor.png"};
    QAction *changeBackgroundColorAct = new QAction{changeBackgroundColorIcon, tr("Change background view's color")};
    changeBackgroundColorAct->setToolTip(tr("Change background's color of view"));
    connect(changeBackgroundColorAct, &QAction::triggered, this, &MainWindow::changeBackgroundViewColor);
    editMenu->addAction(changeBackgroundColorAct);
    editToolBar->addAction(changeBackgroundColorAct);

    // CHANGE GRID COLOR
    QIcon changeGridColorIcon{":/icons/changeGridColor.png"};
    QAction *changeGridColorAct = new QAction{changeGridColorIcon, tr("Change grid's color")};
    changeGridColorAct->setToolTip(tr("Change grid color"));
    connect(changeGridColorAct, &QAction::triggered, this, &MainWindow::changeGridColor);
    editMenu->addAction(changeGridColorAct);
    editToolBar->addAction(changeGridColorAct);

    /////////////////////// TOOL MENU
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = new QToolBar{"Tools", this};
    addToolBar(Qt::LeftToolBarArea, toolsToolBar);
    toolsToolBar->setStyleSheet(toolBarStyle);

    // SELECTION
    QIcon selectIcon{":/icons/selection.png"};
    m_selectAct = new QAction{selectIcon, tr("&Selection Tool"), this};
    m_selectAct->setCheckable(true);
    connect(m_selectAct, &QAction::triggered, this, &MainWindow::selectToolTriggered);
    toolsMenu->addAction(m_selectAct);
    toolsToolBar->addAction(m_selectAct);

    // SELECT ALL
    QAction *selectAllAct = new QAction{tr("Select all"), this};
    selectAllAct->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAct, &QAction::triggered, m_mapView, &MapView::selectAll);
    toolsMenu->addAction(selectAllAct);

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
    connect(this, &MainWindow::toolTriggered, m_mapView, &MapView::toolTriggered);
    connect(m_mapView, &MapView::checkTool, this, &MainWindow::checkedTool);
    m_mapView->setStyleSheet(m_styleRegister[StyleSheet::MapView]);
}

void MainWindow::createMapScene()
{
    m_mapScene->holdStatusBar(statusBar());
    m_mapScene->holdTextureList(m_textureList);

    connect(m_mapScene, &MapScene::mouseMoveAndPressLeft,
            m_mapView, &MapView::mouseMovingAndPressing);
    connect(m_mapScene, &MapScene::itemFocusChange,
            m_mapView, &MapView::itemFocusChanged);
    connect(m_mapScene, &MapScene::clearUndoStack,
            this, &MainWindow::clearUndoStack);

    std::map<NewMapDialog::FieldType, int> defaultMap{};
    defaultMap[NewMapDialog::FieldType::Width]  = 32;
    defaultMap[NewMapDialog::FieldType::Height] = 32;
    defaultMap[NewMapDialog::FieldType::Rows]   = 16;
    defaultMap[NewMapDialog::FieldType::Cols]   = 16;
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
    statusBar()->setStyleSheet(m_styleRegister[StyleSheet::StatusBar]);
}

void MainWindow::createUndoView()
{
    m_undoView = new QUndoView{m_undoStack};
    m_undoView->setWindowTitle(tr("Command List"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);
    m_undoView->setStyleSheet(m_styleRegister[StyleSheet::UndoView]);
}

void MainWindow::createDockWindows()
{
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    const QString styleSheet{m_styleRegister[StyleSheet::DockWidget]};

    m_dockTextureList = new QDockWidget{tr("Texture List"), this};
    m_dockTextureList->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dockTextureList->setWidget(m_textureList);
    m_dockTextureList->setMinimumHeight(500);
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
    m_dockUndoView->setStyleSheet(styleSheet);
    m_dockUndoView->setFloating(false);
    addDockWidget(Qt::RightDockWidgetArea, m_dockUndoView);
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
    // TODO : Saving file can take a little time
    //          -> make a classic progress bar dialog ?
    //          -> or progress bar in status bar ?

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
    emit windowTitleChanged(StringTools::cutFileName(showName) + " - Mapox2D");
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

void MainWindow::registerStyleSheet()
{
    m_styleRegister.registerStyleSheet("menuBarStyle.css"     , StyleSheet::MenuBar);
    m_styleRegister.registerStyleSheet("textureListStyle.css" , StyleSheet::TextureList);
    m_styleRegister.registerStyleSheet("toolBarStyle.css"     , StyleSheet::ToolBar);
    m_styleRegister.registerStyleSheet("mapViewStyle.css"     , StyleSheet::MapView);
    m_styleRegister.registerStyleSheet("undoViewStyle.css"    , StyleSheet::UndoView);
    m_styleRegister.registerStyleSheet("dockWidgetStyle.css"  , StyleSheet::DockWidget);
    m_styleRegister.registerStyleSheet("statusBarStyle.css"   , StyleSheet::StatusBar);
}
