#include "customwindow.h"

CustomWindow::CustomWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setWindowFlag(Qt::FramelessWindowHint);
    w.setWindowFlag(Qt::FramelessWindowHint);
    setCentralWidget(&w);
    createTitleBar();
}

void CustomWindow::minimizeClicked()
{
    qDebug() << "CustomWindow::minimizeClicked";
}

void CustomWindow::maximizeClicked()
{
    qDebug() << "CustomWindow::maximizeClicked";
    const QRect screenGeometry = QGuiApplication::focusWindow()->screen()->availableGeometry();
    qDebug() << "Screen Geometry        = " << screenGeometry;
    qDebug() << "Custom Window Geometry = " << geometry();
}

void CustomWindow::closeClicked()
{
    if(w.close())
        qApp->quit();
}

void CustomWindow::mouseMoveEvent(QMouseEvent *event)
{
    // Info : Mouse Tracking not actived so, this event
    //        is received only if mouse button is pressed
    m_mousePos = QPoint{static_cast<int>(event->screenPos().x()),
                        static_cast<int>(event->screenPos().y())};
    // event->pos() refers to the mouse's pos relative to the window
    if(m_dragRegion.contains(event->pos()) && m_mouseLeftPress)
        move(QPoint{m_mousePos - m_offsetDrag});
    else
        m_mouseLeftPress = false;
}

void CustomWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_mouseLeftPress = true;
        m_offsetDrag = event->pos();
    }
}

void CustomWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_mouseLeftPress = false;
    }
}

void CustomWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_dragRegion = QRect{0, 0,
                         size().width() - (3*m_buttonClose->iconSize().width() + 10),
                         m_titleBar->size().height()};
}

void CustomWindow::createTitleBar()
{
    m_title.setText("Untitled");

    m_titleBar = new QWidget{this};
    m_titleBar->setStyleSheet("QWidget{"
                              "    background    : rgb(60, 60, 60);"
                              "    border        : 1px solid rgb(100, 100, 100);"
                              "    color         : rgb(210, 210, 210);"
                              "}"
                              "QPushButton{"
                              "    border-bottom : 1px solid rgb(100, 100, 100);"
                              "    padding       : 0;"
                              "    margin-left   : 2px;"
                              "    background    : darkgray;"
                              "}"
                              "QPushButton:hover{"
                              "    background    : rgb(50, 130, 150);"
                              "}"
                              "QPushButton:pressed{"
                              "    background    : red;"
                              "}"
                              "QPushButton:flat{"
                              "    border        : 1px solid rgb(100, 100, 100);"
                              "}"
                              "QPushButton:default{"
                              "    border-color  : navy;"
                              "}");


    createButtons();

    QHBoxLayout *hLayout = new QHBoxLayout{};
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(&m_title        , 1, Qt::AlignLeft);
    hLayout->addWidget(m_buttonMinimize, 0, Qt::AlignRight);
    hLayout->addWidget(m_buttonMaximize, 0, Qt::AlignRight);
    hLayout->addWidget(m_buttonClose   , 0, Qt::AlignRight);

    m_titleBar->setLayout(hLayout);
    setMenuWidget(m_titleBar);
}

void CustomWindow::createButtons()
{
    m_buttonMinimize = new QPushButton{this};
    QPixmap minimizeIcon{":/icons/minimize.png"};
    m_buttonMinimize->setIcon(minimizeIcon);
    m_buttonMinimize->setIconSize(minimizeIcon.size());
    m_buttonMinimize->setFlat(true);
    connect(m_buttonMinimize, &QPushButton::clicked,
            this, &CustomWindow::minimizeClicked);

    m_buttonMaximize = new QPushButton{this};
    QPixmap maximizeIcon{":/icons/maximize.png"};
    m_buttonMaximize->setIcon(maximizeIcon);
    m_buttonMaximize->setIconSize(maximizeIcon.size());
    m_buttonMaximize->setFlat(true);
    connect(m_buttonMaximize, &QPushButton::clicked,
            this, &CustomWindow::maximizeClicked);

    m_buttonClose    = new QPushButton{this};
    QPixmap closeIcon{":/icons/close.png"};
    m_buttonClose->setIcon(closeIcon);
    m_buttonClose->setIconSize(closeIcon.size());
    m_buttonClose->setFlat(true);
    connect(m_buttonClose, &QPushButton::clicked,
            this, &CustomWindow::closeClicked);
}
