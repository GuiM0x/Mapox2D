#ifndef MYCENTRAWIDGET_H
#define MYCENTRAWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QMouseEvent>

class MyCentraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyCentraWidget(QWidget *parent = nullptr);

public:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MYCENTRAWIDGET_H
