#include "importspritesheetdialog.h"

ImportSpriteSheetDialog::ImportSpriteSheetDialog(const QPixmap& spriteSheet,
                                                 QWidget *parent)
    : QDialog{parent,
              Qt::CustomizeWindowHint |
              Qt::WindowTitleHint |
              Qt::WindowMinMaxButtonsHint |
              Qt::WindowCloseButtonHint}
{
    setWindowTitle(tr("Sprite Sheet Split Configuration"));
    setSizeGripEnabled(true);

    m_spriteSheet = spriteSheet;

    m_graphicsView.setScene(&m_graphicsScene);
    QGraphicsRectItem *imgItem = new QGraphicsRectItem{QRectF{QPointF{0, 0}, m_spriteSheet.size()}};
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
    if(!m_rowColGroupBox->isChecked() && n != 0){
        const int colsValue = m_spriteSheet.size().width() / n;
        m_spinBoxCols->setValue(colsValue);
        makeLines(colsValue, Line::Vertical);
    }
}

void ImportSpriteSheetDialog::heightValueChanged(int n)
{
    if(!m_rowColGroupBox->isChecked() && n != 0){
        const int rowsValue = m_spriteSheet.size().height() / n;
        m_spinBoxRows->setValue(rowsValue);
        makeLines(rowsValue, Line::Horizontal);
    }
}

void ImportSpriteSheetDialog::rowsValueChanged(int n)
{
    if(!m_sizeGroupBox->isChecked() && n != 0){
        const int heightValue = m_spriteSheet.size().height() / n;
        m_spinBoxHeight->setValue(heightValue);
        makeLines(static_cast<qreal>(heightValue), Line::Horizontal);
    }
}

void ImportSpriteSheetDialog::colsValueChanged(int n)
{
    if(!m_sizeGroupBox->isChecked() && n != 0){
        const int widthValue = m_spriteSheet.size().width() / n;
        m_spinBoxWidth->setValue(widthValue);
        makeLines(static_cast<qreal>(widthValue), Line::Vertical);
    }
}

QList<QPixmap> ImportSpriteSheetDialog::processDial()
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
            values[it->first] = it->second->value();
        }
    }

    return splitSpriteSheet(values);
}

void ImportSpriteSheetDialog::createForm()
{
    createSizeOptionBox();
    createRowColOptionBox();

    QVBoxLayout *leftVLayout = new QVBoxLayout{};
    leftVLayout->addWidget(m_sizeGroupBox);
    leftVLayout->addWidget(m_rowColGroupBox);
    leftVLayout->setAlignment(Qt::AlignVCenter);

    QHBoxLayout *hLayout = new QHBoxLayout{};
    hLayout->addLayout(leftVLayout);
    hLayout->addWidget(&m_graphicsView);

    m_form.addRow(hLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox{QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this};
    buttonBox->addButton("Split !", QDialogButtonBox::AcceptRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    m_form.addRow(buttonBox);
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

void ImportSpriteSheetDialog::makeLines(int count, Line type)
{
    m_graphicsScene.clear();
    QGraphicsRectItem *imgItem = new QGraphicsRectItem{QRectF{QPointF{0, 0}, m_spriteSheet.size()}};
    imgItem->setBrush(QBrush{m_spriteSheet});
    m_graphicsScene.addItem(imgItem);

    const bool horizontal = (type == Line::Horizontal) ? true : false;
    const bool vertical   = !horizontal;
    const QSizeF spriteSheetSize = m_spriteSheet.size();
    if(horizontal){
        m_horizontalLine.clear();
        const qreal spacing = spriteSheetSize.height() / count;
        for(int row = 0; row < count; ++row){
            const QPointF startPoint{0, row*spacing};
            const QPointF endPOint{spriteSheetSize.width(), row*spacing};
            const QLineF line{startPoint, endPOint};
            m_horizontalLine.push_back(line);
            m_graphicsScene.addLine(line);
        }
        if(!m_verticalLine.empty()){
            for(const auto& line :m_verticalLine){
                m_graphicsScene.addLine(line);
            }
        }
    }
    if(vertical){
        m_verticalLine.clear();
        const qreal spacing = spriteSheetSize.width() / count;
        for(int col = 0; col < count; ++col){
            const QPointF startPoint{col*spacing, 0};
            const QPointF endPOint{col*spacing, spriteSheetSize.height()};
            QLineF line{startPoint, endPOint};
            m_verticalLine.push_back(line);
            m_graphicsScene.addLine(line);
        }
        if(!m_horizontalLine.empty()){
            for(const auto& line : m_horizontalLine){
                m_graphicsScene.addLine(line);
            }
        }
    }
}

void ImportSpriteSheetDialog::makeLines(qreal spacing, Line type)
{
    const bool horizontal = (type == Line::Horizontal) ? true : false;
    const bool vertical   = !horizontal;
    if(horizontal){
        const int rows = static_cast<int>(m_spriteSheet.size().height() / spacing);
        makeLines(rows, type);
    }
    if(vertical){
        const int cols = static_cast<int>(m_spriteSheet.size().width() / spacing);
        makeLines(cols, type);
    }
}

void ImportSpriteSheetDialog::createSizeOptionBox()
{
    // CREATE WIDTH SPIN BOX
    m_spinBoxWidth = new QSpinBox{this};
    m_spinBoxWidth->setRange(0, 9999);
    connect(m_spinBoxWidth, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::widthValueChanged);
    m_fields[FieldType::Width] = m_spinBoxWidth;
    QLabel *labelWidth = new QLabel{"Width", this};
    QHBoxLayout *labelSpinWidthLayout = new QHBoxLayout{};
    labelSpinWidthLayout->addWidget(labelWidth);
    labelSpinWidthLayout->addWidget(m_spinBoxWidth);

    // CREATE HEIGHT SPIN BOX
    m_spinBoxHeight = new QSpinBox{this};
    m_spinBoxHeight->setRange(0, 9999);
    connect(m_spinBoxHeight, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::heightValueChanged);
    m_fields[FieldType::Height] = m_spinBoxHeight;
    QLabel *labelHeight = new QLabel{"Height", this};
    QHBoxLayout *labelSpinHeightLayout = new QHBoxLayout{};
    labelSpinHeightLayout->addWidget(labelHeight);
    labelSpinHeightLayout->addWidget(m_spinBoxHeight);

    // CREATE LAYOUT FOR SPIN BOX
    QVBoxLayout *vboxSize = new QVBoxLayout{};
    vboxSize->addLayout(labelSpinWidthLayout);
    vboxSize->addLayout(labelSpinHeightLayout);

    // CREATE GROUP BOX FOR LAYOUT
    m_sizeGroupBox = new QGroupBox{tr("Tile size : "), this};
    m_sizeGroupBox->setCheckable(true);
    m_sizeGroupBox->setChecked(true);
    m_sizeGroupBox->setLayout(vboxSize);
    m_sizeGroupBox->setMinimumWidth(150);
    connect(m_sizeGroupBox, &QGroupBox::clicked,
            this, &ImportSpriteSheetDialog::sizeGroupBoxClicked);
}

void ImportSpriteSheetDialog::createRowColOptionBox()
{
    // CREATE ROWS SPIN BOX
    m_spinBoxRows = new QSpinBox{this};
    m_spinBoxRows->setRange(0, 999);
    connect(m_spinBoxRows, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::rowsValueChanged);
    m_fields[FieldType::Rows] = m_spinBoxRows;
    QLabel *labelRows = new QLabel{"Rows", this};
    QHBoxLayout *labelSpinRowLayout = new QHBoxLayout{};
    labelSpinRowLayout->addWidget(labelRows);
    labelSpinRowLayout->addWidget(m_spinBoxRows);

    // CREATE COLS SPIN BOX
    m_spinBoxCols = new QSpinBox{this};
    m_spinBoxCols->setRange(0, 999);
    connect(m_spinBoxCols, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ImportSpriteSheetDialog::colsValueChanged);
    m_fields[FieldType::Cols] = m_spinBoxCols;
    QLabel *labelCols = new QLabel{"Cols", this};
    QHBoxLayout *labelSpinColLayout = new QHBoxLayout{};
    labelSpinColLayout->addWidget(labelCols);
    labelSpinColLayout->addWidget(m_spinBoxCols);

    // CREATE LAYOUT FOR SPIN BOX
    QVBoxLayout *vboxrowCol = new QVBoxLayout{};
    vboxrowCol->addLayout(labelSpinRowLayout);
    vboxrowCol->addLayout(labelSpinColLayout);

    // CREATE GROUP BOX FOR LAYOUT
    m_rowColGroupBox = new QGroupBox{tr("Tile Number : "), this};
    m_rowColGroupBox->setCheckable(true);
    m_rowColGroupBox->setChecked(false);
    m_rowColGroupBox->setLayout(vboxrowCol);
    m_rowColGroupBox->setMinimumWidth(150);
    connect(m_rowColGroupBox, &QGroupBox::clicked,
            this, &ImportSpriteSheetDialog::rowColGroupBoxClicked);
}

QList<QPixmap> ImportSpriteSheetDialog::splitSpriteSheet(std::map<FieldType, int>& values)
{
    const int tileWidth = values[FieldType::Width];
    const int tileHeight = values[FieldType::Height];
    const int rows = values[FieldType::Rows];
    const int cols = values[FieldType::Cols];

    QList<QPixmap> tiles{};

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            const QPixmap rectCopy =
                    m_spriteSheet.copy(j*tileWidth, i*tileHeight, tileWidth, tileHeight);
            tiles.append(rectCopy);
        }
    }

    return tiles;
}
