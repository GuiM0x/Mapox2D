#ifndef IMPORTSPRITESHEETDIALOG_H
#define IMPORTSPRITESHEETDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QIntValidator>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

enum class FieldType{
    Width,
    Height,
    Rows,
    Cols
};

class ImportSpriteSheetDialog : public QDialog
{
public:  
    ImportSpriteSheetDialog(const QPixmap& spriteSheet,
                            QWidget *parent = nullptr);

private slots:
    void sizeGroupBoxClicked(bool clicked);
    void rowColGroupBoxClicked(bool clicked);
    void widthValueChanged(int n);
    void heightValueChanged(int n);
    void rowsValueChanged(int n);
    void colsValueChanged(int n);

public:
    std::map<FieldType, int> processDial();

private:
    void createForm();
    void createButtonBox();
    bool validInput();

private:
    QFormLayout                     m_form{this};
    std::map<FieldType, QSpinBox*>  m_fields{};
    QPixmap                         m_spriteSheet{};
    QGraphicsView                   m_graphicsView{this};
    QGraphicsScene                  m_graphicsScene{};
    QGroupBox                      *m_sizeGroupBox{nullptr};
    QGroupBox                      *m_rowColGroupBox{nullptr};
    QSpinBox                       *m_spinBoxWidth{nullptr};
    QSpinBox                       *m_spinBoxHeight{nullptr};
    QSpinBox                       *m_spinBoxRows{nullptr};
    QSpinBox                       *m_spinBoxCols{nullptr};
};

#endif // IMPORTSPRITESHEETDIALOG_H
