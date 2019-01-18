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
        }
        setCurrentFile(QString{});
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
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                          tr("Images (*.png *.jpg *.bmp)"));
    if(!fileName.isEmpty())
        m_textureList->addTexture(fileName);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        int index = m_mapScene->currentTile();
        if(m_mapScene->canFillTile(index)){
            QUndoCommand *fillCommand = new FillTileCommand{m_mapScene};
            m_undoStack->push(fillCommand);
            /*connect(static_cast<FillTileCommand*>(fillCommand), &FillTileCommand::docModified,
                    this, &MainWindow::docWasModified);*/
        }
    }
}

void MainWindow::createActions()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *newAct = new QAction{tr("&New map"), this};
    newAct->setShortcut(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new map"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newMap);
    fileMenu->addAction(newAct);

    QAction *openAct = new QAction{tr("&Open..."), this};
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip(tr("Open existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    QAction *saveAct = new QAction{tr("&Save"), this};
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save current file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);

    QAction *saveAsAct = new QAction{tr("&Save as..."), this};
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save current file as..."));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    // Edit Menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    QAction *undoAct = m_undoStack->createUndoAction(this, tr("&Undo"));
    undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAct);

    QAction *redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);

    // Help Menu
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
    m_mapView->setBackgroundBrush(QBrush{Qt::gray});
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
    m_documentModified = false; //??  GOOD HERE ??
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
    /*m_addTextureButton->setMinimumWidth(300);
    m_addTextureButton->setMaximumWidth(300);*/
    m_addTextureButton->setFixedWidth(300);
    centralGridLayout->addWidget(m_textureList, 1, 1);
    /*m_textureList->setMinimumWidth(300);
    m_textureList->setMaximumWidth(300);*/
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
    QString showName = m_currentFile;
    if(m_currentFile.isEmpty())
        showName = "Untitled";
    setWindowFilePath(showName);
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
