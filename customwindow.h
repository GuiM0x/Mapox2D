#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include "mainwindow.h"

#include <QMainWindow>
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
        CLOSE
    };

public:
    explicit CustomWindow(QWidget *parent = nullptr);

private slots:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void createTitleBar();
    void createButtons();
    void createLayout();

private:
    MainWindow    w{};
    QWidget      *m_titleBar{nullptr};
    QLabel        m_title{};
    QPushButton  *m_buttonMinimize{nullptr};
    QPushButton  *m_buttonMaximize{nullptr};
    QPushButton  *m_buttonClose{nullptr};
    QPoint        m_offsetDrag{};
    QPoint        m_mousePos{};
    QRect         m_dragRegion{};
    bool          m_mouseLeftPress{false};
    QRect         m_oldGeometry{};
};

#endif // CUSTOMWINDOW_H
