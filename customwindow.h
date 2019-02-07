#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include "mainwindow.h"

#include <QMainWindow>
#include <QIcon>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWindow>
#include <QScreen>

class CustomWindow : public QMainWindow
{
    Q_OBJECT

    enum class ButtonType{
        MINIMIZE,
        MAXIMIZE,
        CLOSE,
        MAX_BUTTONS
    };

public:
    explicit CustomWindow(QWidget *parent = nullptr);

private slots:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();
    // Connected with signal MainWindow::windowTitleChanged(...)
    void windowTitleChanged(const QString& title);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void createIconWidget();
    void createTitleBar();
    void createButtons();

private:
    MainWindow      w{};
    QLabel         *m_icon{nullptr};
    QLabel         *m_title{nullptr};
    QWidget        *m_titleBar{nullptr};
    QPushButton    *m_buttonMinimize{nullptr};
    QPushButton    *m_buttonMaximize{nullptr};
    QPushButton    *m_buttonClose{nullptr};
    QPoint          m_offsetDrag{};
    QPoint          m_mousePos{};
    QRect           m_dragRegion{};
    bool            m_mouseLeftPress{false};
    bool            m_maximized{false};
};

#endif // CUSTOMWINDOW_H
