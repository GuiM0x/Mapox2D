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

class NewMapDialog : public QDialog
{
    Q_OBJECT

public:
    NewMapDialog(QWidget *parent = nullptr);

public:
    std::map<QString, int> processDial();

private slots:
    void textHasBeenEdited(const QString& text);

private:
    void createForm();
    void createButtonBox();
    bool validInput();

private:
    QIntValidator                 m_validatorSizeTile{0, 999};
    QIntValidator                 m_validatorCount{0, 500};
    QFormLayout                   m_form{this};
    std::map<QString, QLineEdit*> m_fields{};
};

#endif // NEWMAPDIALOG_H
