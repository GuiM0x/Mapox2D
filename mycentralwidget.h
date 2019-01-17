#ifndef MYCENTRALWIDGET_H
#define MYCENTRALWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QMouseEvent>

class MyCentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyCentralWidget(QWidget *parent = nullptr);

public:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MYCENTRALWIDGET_H
