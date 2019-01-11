#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      m_centralWidget{new QWidget{this}},
      m_mapView{new MapView{this}},
      m_mapScene{new MapScene{m_mapView}},
      m_textureList{new TextureList{this}},
      m_addTextureButton{new QPushButton{this}}
{
    setCentralWidget(m_centralWidget);

    createActions();
    createMapView();
    createMapScene();
    createGridLayout();
    createStatusBar();

    m_addTextureButton->setText(tr("Add Texture"));
    connect(m_addTextureButton, &QPushButton::clicked, this, &MainWindow::openTexture);
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // In the future, it will be used to check if doc was saved or not.
    // If not, the user will be invite to save, else the window will be closed
    const QMessageBox::StandardButton ret = QMessageBox::warning(this,
                                                                 tr("Application"),
                                                                 tr("Do you really want to quit ?"),
                                                                 QMessageBox::Ok | QMessageBox::Cancel);
    if(ret == QMessageBox::Ok)
        event->accept();
    else
        event->ignore();
}

void MainWindow::newMap()
{
    // TODO : Before process new map, check if save is needed and if user want it

    NewMapDialog dial{this};
    // NewMapDialog::processDial return a map<Qstring, int> that contains values :
    // ["tileWidth"]  => val(int)
    // ["tileHeight"] => val(int)
    // ["totalRows"]  => val(int)
    // ["totalCols"]  => val(int)
    auto values = dial.processDial();
    if(!values.empty())
        createMapScene(values);
}

void MainWindow::open()
{
    qDebug() << "Slots open() triggered";
}

void MainWindow::save()
{
    qDebug() << "Slots save() triggered";
}

void MainWindow::saveAs()
{
    qDebug() << "Slots saveAs() triggered";
}

void MainWindow::docWasModified()
{
    qDebug() << "Slots docWasModified() triggered";
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
    m_textureList->addTexture(fileName);
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
    m_mapView->setBackgroundBrush(QBrush{QColor{175, 175, 175}});
}

void MainWindow::createMapScene()
{
    std::map<QString, int> defaultMap{};
    defaultMap["tileWidth"]  = 32;
    defaultMap["tileHeight"] = 32;
    defaultMap["totalRows"]  = 6;
    defaultMap["totalCols"]  = 6;
    createMapScene(defaultMap);
}

void MainWindow::createMapScene(std::map<QString, int>& values)
{
    m_mapScene->holdStatusBar(statusBar());

    m_mapScene->createMatrix(values["tileWidth"],
                             values["tileHeight"],
                             values["totalRows"],
                             values["totalCols"]);

    m_mapView->setSceneRect(m_mapScene->itemsBoundingRect());
    m_mapView->resetTransform();
}

void MainWindow::createGridLayout()
{
    QGridLayout *centralGridLayout = new QGridLayout{};
    centralGridLayout->addWidget(m_mapView, 0, 0, 2, 1);
    centralGridLayout->addWidget(m_addTextureButton, 0, 1);
    m_addTextureButton->setMaximumWidth(350);
    centralGridLayout->addWidget(m_textureList, 1, 1);
    m_textureList->setMaximumWidth(350);
    m_centralWidget->setLayout(centralGridLayout);
}

void MainWindow::createStatusBar()
{
    // Status Bar is automatically created the first time statusBar() is called
    // This function return a pointer to the main window's qstatusbar
    statusBar()->showMessage(tr("Ready !"));
}

void MainWindow::maybeSave()
{

}
