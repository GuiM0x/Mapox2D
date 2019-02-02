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
#include <QGraphicsLineItem>

class ImportSpriteSheetDialog : public QDialog
{
    enum class Line{
        Horizontal,
        Vertical
    };

public:
    enum class FieldType{
        Width,
        Height,
        Rows,
        Cols
    };

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
    QList<QPixmap> processDial();

private:
    void createForm();
    void createButtonBox();
    bool validInput();
    void makeLines(int count, Line type = Line::Horizontal);
    void makeLines(qreal spacing, Line type = Line::Horizontal);
    void createSizeOptionBox();
    void createRowColOptionBox();
    QList<QPixmap> splitSpriteSheet(std::map<FieldType, int>& values);


private:
    QFormLayout                      m_form{this};
    std::map<FieldType, QSpinBox*>   m_fields{};
    QPixmap                          m_spriteSheet{};
    QGraphicsView                    m_graphicsView{this};
    QGraphicsScene                   m_graphicsScene{};
    QGroupBox                       *m_sizeGroupBox{nullptr};
    QGroupBox                       *m_rowColGroupBox{nullptr};
    QSpinBox                        *m_spinBoxWidth{nullptr};
    QSpinBox                        *m_spinBoxHeight{nullptr};
    QSpinBox                        *m_spinBoxRows{nullptr};
    QSpinBox                        *m_spinBoxCols{nullptr};
    std::vector<QLineF>              m_horizontalLine{};
    std::vector<QLineF>              m_verticalLine{};
};

#endif // IMPORTSPRITESHEETDIALOG_H
