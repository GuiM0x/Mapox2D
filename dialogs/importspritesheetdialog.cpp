#include "importspritesheetdialog.h"

ImportSpriteSheetDialog::ImportSpriteSheetDialog(const QPixmap& spriteSheet,
                                                 QWidget *parent)
    : QDialog{parent,
              Qt::CustomizeWindowHint |
              Qt::WindowTitleHint |
              Qt::WindowCloseButtonHint}
{
    setWindowTitle(tr("Sprite Sheet Split Configuration"));

    m_spriteSheet = spriteSheet;

    m_graphicsView.setScene(&m_graphicsScene);
    QGraphicsRectItem *imgItem = new QGraphicsRectItem{QRectF{QPointF{0, 0},m_spriteSheet.size()}};
    imgItem->setBrush(QBrush{m_spriteSheet});
    m_graphicsScene.addItem(imgItem);

    createForm();
}

void ImportSpriteSheetDialog::sizeGroupBoxClicked(bool clicked)
{
    m_rowColGroupBox->setChecked(!clicked);
}

void ImportSpriteSheetDialog::rowColGroupBoxClicked(bool clicked)
{
    m_sizeGroupBox->setChecked(!clicked);
}

void ImportSpriteSheetDialog::widthValueChanged(int n)
{
    qDebug() << "ImportSpriteSheetDialog::widthValueChanged - " << n;
    if(!m_rowColGroupBox->isChecked())
        m_spinBoxCols->setValue(n+1);
}

void ImportSpriteSheetDialog::heightValueChanged(int n)
{
    qDebug() << "ImportSpriteSheetDialog::heightValueChanged - " << n;
    if(!m_rowColGroupBox->isChecked())
        m_spinBoxRows->setValue(n+1);
}

void ImportSpriteSheetDialog::rowsValueChanged(int n)
{
    qDebug() << "ImportSpriteSheetDialog::rowsValueChanged - " << n;
    if(!m_sizeGroupBox->isChecked())
        m_spinBoxHeight->setValue(n+1);
}

void ImportSpriteSheetDialog::colsValueChanged(int n)
{
    qDebug() << "ImportSpriteSheetDialog::colsValueChanged - " << n;
    if(!m_sizeGroupBox->isChecked())
        m_spinBoxWidth->setValue(n+1);
}

std::map<FieldType, int> ImportSpriteSheetDialog::processDial()
{
    QLabel *invalidFieldInfo = new QLabel{this};
    // Exec dialog while input are not valid except if dial is canceled
    do{
        exec();
        if(!validInput() && result() == QDialog::Accepted){
            if(invalidFieldInfo->text().isEmpty()){
                invalidFieldInfo->setText(tr("A field cannot be equal to 0 !"));
                m_form.addRow(invalidFieldInfo);
            }
        }
    }while(!validInput() && result() == QDialog::Accepted);


    std::map<FieldType, int> values{};
    // Ok, we can process if all input are good and user say Ok
    if(result() == QDialog::Accepted){
        for(auto it = std::begin(m_fields); it != std::end(m_fields); ++it){
            values[it->first] = it->second->text().toInt();
        }
    }

    return values;
}

void ImportSpriteSheetDialog::createForm()
{
    m_form.addRow(&m_graphicsView);

    m_sizeGroupBox = new QGroupBox{"By size : ", this};
    m_sizeGroupBox->setCheckable(true);
    m_sizeGroupBox->setChecked(true);
    QVBoxLayout *vboxSize = new QVBoxLayout{this};
    m_spinBoxWidth = new QSpinBox{this};
    m_spinBoxWidth->setRange(0, 500);
    connect(m_spinBoxWidth, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::widthValueChanged);
    m_fields[FieldType::Width] = m_spinBoxWidth;
    m_spinBoxHeight = new QSpinBox{this};
    m_spinBoxHeight->setRange(0, 500);
    connect(m_spinBoxHeight, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::heightValueChanged);
    m_fields[FieldType::Height] = m_spinBoxHeight;
    vboxSize->addWidget(m_spinBoxWidth);
    vboxSize->addWidget(m_spinBoxHeight);
    m_sizeGroupBox->setLayout(vboxSize);
    connect(m_sizeGroupBox, &QGroupBox::clicked,
            this, &ImportSpriteSheetDialog::sizeGroupBoxClicked);

    m_form.addRow(m_sizeGroupBox);

    m_rowColGroupBox = new QGroupBox{"By number of rows and cols : ", this};
    m_rowColGroupBox->setCheckable(true);
    m_rowColGroupBox->setChecked(false);
    QVBoxLayout *vboxrowCol = new QVBoxLayout{this};
    m_spinBoxRows = new QSpinBox{this};
    m_spinBoxRows->setRange(0, 999);
    connect(m_spinBoxRows, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::rowsValueChanged);
    m_fields[FieldType::Rows] = m_spinBoxRows;
    m_spinBoxCols = new QSpinBox{this};
    m_spinBoxCols->setRange(0, 999);
    connect(m_spinBoxCols, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::colsValueChanged);
    m_fields[FieldType::Cols] = m_spinBoxCols;
    vboxrowCol->addWidget(m_spinBoxRows);
    vboxrowCol->addWidget(m_spinBoxCols);
    m_rowColGroupBox->setLayout(vboxrowCol);
    connect(m_rowColGroupBox, &QGroupBox::clicked,
            this, &ImportSpriteSheetDialog::rowColGroupBoxClicked);

    m_form.addRow(m_rowColGroupBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, this};
    m_form.addRow(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool ImportSpriteSheetDialog::validInput()
{
    bool valid{true};
    for(auto it = std::begin(m_fields); it != std::end(m_fields); ++it){
        if(it->second->value() == 0){
            valid = false;
            break;
        }
    }
    return valid;
}
