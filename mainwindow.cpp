#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      m_centralWidget{new QWidget{this}},
      m_mapView{new MapView{this}},
      m_mapScene{new MapScene{m_mapView}},
      m_textureList{new TextureList{this}},
      m_addTextureButton{new QPushButton{this}},
      m_undoStack{new QUndoStack{this}}
{
    setCentralWidget(m_centralWidget);
    setWindowFilePath("Untitled  - Mapox2D");

    createActions();
    createMapView();
    createMapScene();
    createGridLayout();
    createStatusBar();
    createUndoView();

    m_addTextureButton->setText(tr("Add Texture"));

    connect(m_addTextureButton, &QPushButton::clicked, this, &MainWindow::openTexture);
    connect(m_textureList, &QListWidget::itemClicked, m_mapScene, &MapScene::currentTextureSelectedInList);
    connect(m_undoStack, &QUndoStack::indexChanged, this, &MainWindow::docWasModified);
    connect(m_textureList, &TextureList::docModified, this, &MainWindow::docWasModified);
    connect(this, &MainWindow::selectToolActived, m_mapView, &MapView::selectToolActived);
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
            createMapScene(values);
            setCurrentFile(QString{});
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

void MainWindow::fillAll()
{
    const QString textureName = m_mapScene->currentTextureName();
    if(!textureName.isEmpty()){
        QUndoCommand *fillAllCommand = new FillAllCommand{m_mapScene, textureName};
        m_undoStack->push(fillAllCommand);
    }
}

void MainWindow::quit()
{
    if(maybeSave())
        qApp->quit();
}

void MainWindow::selectToolChecked(bool checked)
{
    if(checked)
        emit selectToolActived(true);
    else
        emit selectToolActived(false);
}

void MainWindow::createActions()
{
    ///////////////// File Menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    QIcon newIcon{":/icons/newFile.png"};
    QAction *newAct = new QAction{newIcon, tr("&New map"), this};
    newAct->setShortcut(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new map"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newMap);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    fileMenu->addSeparator();

    QIcon openIcon{":/icons/openFile.png"};
    QAction *openAct = new QAction{openIcon, tr("&Open..."), this};
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip(tr("Open existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    QIcon saveIcon{":/icons/saveFile.png"};
    QAction *saveAct = new QAction{saveIcon, tr("&Save"), this};
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save current file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();

    QAction *saveAsAct = new QAction{tr("&Save as..."), this};
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save current file as..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    QAction *quitAct = new QAction{tr("&Quit"), this};
    quitAct->setShortcut(QKeySequence{tr("Ctrl+Q")});
    quitAct->setStatusTip(tr("Shutdown Mapox2D"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::quit);
    fileMenu->addAction(quitAct);

    ///////////////// Edit Menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar("Edit");

    QIcon undoIcon{":/icons/undo.png"};
    QAction *undoAct = m_undoStack->createUndoAction(this, tr("&Undo"));
    undoAct->setIcon(undoIcon);
    undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAct);
    editToolBar->addAction(undoAct);

    QIcon redoIcon{":/icons/redo.png"};
    QAction *redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    redoAct->setIcon(redoIcon);
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addSeparator();

    editMenu->addSeparator();

    QAction *fillAllAct = new QAction{tr("F&ill with current texture"), this};
    fillAllAct->setShortcut(QKeySequence{tr("Ctrl+,")});
    fillAllAct->setStatusTip(tr("Fill the grid with selected texture in list"));
    connect(fillAllAct, &QAction::triggered, this, &MainWindow::fillAll);
    editMenu->addAction(fillAllAct);

    ///////////////// Tools Menu
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = addToolBar("Tools");

    QIcon selectIcon{":/icons/selection.png"};
    QAction *selectAct = new QAction{selectIcon, tr("&Selection Tool"), this};
    selectAct->setStatusTip(tr("Rectangular selection"));
    selectAct->setCheckable(true);
    connect(selectAct, &QAction::triggered, this, &MainWindow::selectToolChecked);
    toolsMenu->addAction(selectAct);
    toolsToolBar->addAction(selectAct);

    ///////////////// Help Menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = new QAction{tr("&About"), this};
    aboutAct->setStatusTip(tr("About this software"));
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
    m_mapView->holdUndoStack(m_undoStack);
}

void MainWindow::createMapScene()
{
    m_mapScene->holdStatusBar(statusBar());
    m_mapScene->holdTextureList(m_textureList);
    connect(m_mapScene, &MapScene::mouseMoveAndPressLeft,
            m_mapView, &MapView::mouseMovingAndPressing);

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
    m_mapScene->createMatrix(values["tileWidth"],
                             values["tileHeight"],
                             values["totalRows"],
                             values["totalCols"]);

    m_mapView->reset(m_mapScene->itemsBoundingRect());
}

void MainWindow::createGridLayout()
{
    QGridLayout *centralGridLayout = new QGridLayout{};
    centralGridLayout->addWidget(m_mapView, 0, 0, 2, 1);
    centralGridLayout->addWidget(m_addTextureButton, 0, 1);
    m_addTextureButton->setFixedWidth(300);
    centralGridLayout->addWidget(m_textureList, 1, 1);
    m_textureList->setFixedWidth(300);
    m_centralWidget->setLayout(centralGridLayout);
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
    qDebug() << "MainWindow::saveFile(...) : m_documentModified = false";

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
    qDebug() << "MainWindow::loadFile(...) : m_documentModified = false";
}
