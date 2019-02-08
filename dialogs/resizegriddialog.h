#ifndef RESIZEGRIDDIALOG_H
#define RESIZEGRIDDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>

class ResizeGridDialog : public QDialog
{
    using Rows = int;
    using Cols = int;

public:
    ResizeGridDialog(int mapRows, int mapCols,
                     QWidget *parent = nullptr);

public:
    std::pair<Rows, Cols> processDial();

private:
    void createSpinBox();
    void createForm();
    bool validInput() const;

private:
    QSpinBox     *m_spinRow{nullptr};
    QSpinBox     *m_spinCol{nullptr};
    QGroupBox    *m_groupBox{nullptr};
    QFormLayout   m_form{this};
    const int     m_mapRows{};
    const int     m_mapCols{};
};

#endif // RESIZEGRIDDIALOG_H
