#include "newmapdialog.h"

NewMapDialog::NewMapDialog(QWidget *parent)
    : QDialog{parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint}
{
    setWindowTitle(tr("Create a new map"));
    createForm();
}

std::map<NewMapDialog::FieldType, int> NewMapDialog::processDial()
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
    return values;
}

void NewMapDialog::createForm()
{
    createSizeOptionBox();
    createRowColOptionBox();

    QVBoxLayout *vLayout = new QVBoxLayout{};
    vLayout->addWidget(m_sizeGroupBox);
    vLayout->addWidget(m_rowColGroupBox);
    vLayout->setAlignment(Qt::AlignVCenter);

    m_form.addRow(vLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, this};
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    m_form.addRow(buttonBox);
}

bool NewMapDialog::validInput()
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

void NewMapDialog::createSizeOptionBox()
{
    // CREATE WIDTH SPIN BOX
    m_spinBoxWidth = new QSpinBox{this};
    m_spinBoxWidth->setRange(1, 500);
    m_spinBoxWidth->selectAll();
    m_fields[FieldType::Width] = m_spinBoxWidth;
    QLabel *labelWidth = new QLabel{"Width", this};
    QHBoxLayout *labelSpinWidthLayout = new QHBoxLayout{};
    labelSpinWidthLayout->addWidget(labelWidth);
    labelSpinWidthLayout->addWidget(m_spinBoxWidth);

    // CREATE HEIGHT SPIN BOX
    m_spinBoxHeight = new QSpinBox{this};
    m_spinBoxHeight->setRange(1, 500);
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
    m_sizeGroupBox->setLayout(vboxSize);
}

void NewMapDialog::createRowColOptionBox()
{
    // CREATE ROWS SPIN BOX
    m_spinBoxRows = new QSpinBox{this};
    m_spinBoxRows->setRange(1, 999);
    m_fields[FieldType::Rows] = m_spinBoxRows;
    QLabel *labelRows = new QLabel{"Rows", this};
    QHBoxLayout *labelSpinRowLayout = new QHBoxLayout{};
    labelSpinRowLayout->addWidget(labelRows);
    labelSpinRowLayout->addWidget(m_spinBoxRows);

    // CREATE COLS SPIN BOX
    m_spinBoxCols = new QSpinBox{this};
    m_spinBoxCols->setRange(1, 999);
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
    m_rowColGroupBox->setLayout(vboxrowCol);
}
