#ifndef LOADINGMAPDIALOG_H
#define LOADINGMAPDIALOG_H

#include <QDebug>
#include <QProgressDialog>

class LoadingMapDialog : public QProgressDialog
{
public:
    LoadingMapDialog(int totalTiles,
                     const QString& labelText,
                     const QString& cancelButton,
                     int minPercent = 0,
                     int maxPercent = 0,
                     QWidget *parent = nullptr,
                     Qt::WindowFlags f = Qt::Dialog);

public:
    void updateBar(int index);

private:
    int m_totalTiles{};
    int m_steps{};
};

#endif // LOADINGMAPDIALOG_H
