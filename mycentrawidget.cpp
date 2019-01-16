#include "mycentrawidget.h"

MyCentraWidget::MyCentraWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void MyCentraWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*qDebug() << QString::number(event->x()) +
                ", " + QString::number(event->y());*/
}
