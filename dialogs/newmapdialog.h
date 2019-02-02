#ifndef NEWMAPDIALOG_H
#define NEWMAPDIALOG_H

#include <map>
#include <QDebug>
#include <QDialog>
#include <QIntValidator>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QGroupBox>

class NewMapDialog : public QDialog
{
    Q_OBJECT

public:
    enum class FieldType{
        Width,
        Height,
        Rows,
        Cols
    };

public:
    NewMapDialog(QWidget *parent = nullptr);

public:
    std::map<FieldType, int> processDial();

private:
    void createForm();
    void createButtonBox();
    bool validInput();
    void createSizeOptionBox();
    void createRowColOptionBox();

private:
    QFormLayout                     m_form{this};
    std::map<FieldType, QSpinBox*>  m_fields{};
    QSpinBox                       *m_spinBoxWidth{nullptr};
    QSpinBox                       *m_spinBoxHeight{nullptr};
    QSpinBox                       *m_spinBoxRows{nullptr};
    QSpinBox                       *m_spinBoxCols{nullptr};
    QGroupBox                      *m_sizeGroupBox{nullptr};
    QGroupBox                      *m_rowColGroupBox{nullptr};
};

#endif // NEWMAPDIALOG_H
