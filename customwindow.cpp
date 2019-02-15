#include "customwindow.h"

CustomWindow::CustomWindow(QWidget *parent)
    : QMainWindow{parent},
      m_title{new QLabel{this}},
      m_buttonMinimize{new QPushButton{this}},
      m_buttonMaximize{new QPushButton{this}},
      m_buttonClose{new QPushButton{this}}
{
    setWindowFlag(Qt::FramelessWindowHint);
    w.setWindowFlag(Qt::FramelessWindowHint);
    setCentralWidget(&w);
    m_title->setText("Untitled - Mapox2D");
    createIconWidget();
    createButtons();
    createTitleBar();

    connect(&w, &MainWindow::windowTitleChanged,
            this, &CustomWindow::windowTitleChanged);
}

void CustomWindow::minimizeClicked()
{
    setWindowState(Qt::WindowMinimized);
}

void CustomWindow::maximizeClicked()
{
    setWindowState(windowState() ^ Qt::WindowFullScreen);
    m_maximized = !m_maximized;
}

void CustomWindow::closeClicked()
{
    if(w.close())
        qApp->quit();
}

void CustomWindow::windowTitleChanged(const QString& title)
{
    assert(m_title != nullptr);
    m_title->setText(title);
}

void CustomWindow::mouseMoveEvent(QMouseEvent *event)
{
    // Infos :  - Mouse Tracking is not actived so, this event
    //            is received only if mouse button is pressed
    //          - event->screenPos() : refers to the global desktop pos
    //          - event->pos()       : refers to the mouse's pos relative to the widget

    const QPoint mousePos = QPoint{static_cast<int>(event->screenPos().x()),
                                   static_cast<int>(event->screenPos().y())};

    //          - Even if this event is received only when mouse's boutton is pressed,
    //            we still need to check if mouse button is pressed (especially the left button).
    //            It's usefull to keep only one button to drag action and we can stop the drag if the user
    //            press mouse button out of drag region rectangle and then move into it (in this case,
    //            if we don't check mouse button state, the window will be moved and we don't want that :)).

    if(m_dragRegion.contains(event->pos()) && m_mouseLeftPress){
        move(QPoint{mousePos - m_offsetDrag});
    } else {
        m_mouseLeftPress = false;
    }
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

void CustomWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_dragRegion.contains(event->pos())){
        maximizeClicked();
    }
}

void CustomWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    const int maxButton = static_cast<int>(ButtonType::MAX_BUTTONS);
    m_dragRegion = QRect{0, 0,
                         size().width() - ((maxButton * m_buttonClose->iconSize().width()) + 10),
                         m_titleBar->size().height()};

    // TO DO : When  maximized clicked, resizeEvent is called along the process
    //             -> make temporized transition
}

void CustomWindow::createTitleBar()
{
    m_titleBar = new QWidget{this};
    m_titleBar->setStyleSheet("QWidget{"
                              "    background    : rgb(30, 30, 30);"
                              "    border        : 1px solid rgb(100, 100, 100);"
                              "    color         : rgb(210, 210, 210);"
                              "}"
                              "QLabel{"
                              "    border-top    : 1px solid rgb(100, 100, 100);"
                              "    border-bottom : 1px solid rgb(100, 100, 100);"
                              "    border-left   : none;"
                              "    border-right  : none;"
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
                              "    padding       : 2px -2px -2px 2px;"
                              "}"
                              "QPushButton:flat{"
                              "    border        : 1px solid rgb(100, 100, 100);"
                              "}"
                              "QPushButton:default{"
                              "    border-color  : navy;"
                              "}");

    QHBoxLayout *hLayout = new QHBoxLayout{};
    hLayout->setSpacing(0);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(m_icon          , 0, Qt::AlignLeft);
    hLayout->addWidget(m_title         , 1, Qt::AlignLeft);
    hLayout->addWidget(m_buttonMinimize, 0, Qt::AlignRight);
    hLayout->addWidget(m_buttonMaximize, 0, Qt::AlignRight);
    hLayout->addWidget(m_buttonClose   , 0, Qt::AlignRight);

    m_titleBar->setLayout(hLayout);
    setMenuWidget(m_titleBar);
}

void CustomWindow::createButtons()
{
    QPixmap minimizeIcon{":/icons/minimize.png"};
    m_buttonMinimize->setIcon(minimizeIcon);
    m_buttonMinimize->setIconSize(minimizeIcon.size());
    m_buttonMinimize->setFlat(true);
    connect(m_buttonMinimize, &QPushButton::clicked,
            this, &CustomWindow::minimizeClicked);

    QPixmap maximizeIcon{":/icons/maximize.png"};
    m_buttonMaximize->setIcon(maximizeIcon);
    m_buttonMaximize->setIconSize(maximizeIcon.size());
    m_buttonMaximize->setFlat(true);
    connect(m_buttonMaximize, &QPushButton::clicked,
            this, &CustomWindow::maximizeClicked);

    QPixmap closeIcon{":/icons/close.png"};
    m_buttonClose->setIcon(closeIcon);
    m_buttonClose->setIconSize(closeIcon.size());
    m_buttonClose->setFlat(true);
    connect(m_buttonClose, &QPushButton::clicked,
            this, &CustomWindow::closeClicked);
}

void CustomWindow::createIconWidget()
{
    m_icon = new QLabel{this};
    QPixmap icon{":/icons/icon2D.png"};
    icon = icon.scaled(16, 16);
    m_icon->setPixmap(icon);
    m_icon->setStyleSheet("QLabel{"
                          "    margin-left  : 3px;"
                          "    margin-right : 3px;"
                          "}");
}
