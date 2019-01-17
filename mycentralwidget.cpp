#include "mycentralwidget.h"

MyCentralWidget::MyCentralWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void MyCentralWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*qDebug() << QString::number(event->x()) +
                ", " + QString::number(event->y());*/
}
