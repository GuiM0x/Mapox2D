#include "resizegriddialog.h"

ResizeGridDialog::ResizeGridDialog(int mapRows, int mapCols,
                                   QWidget *parent)
    : QDialog{parent},
      m_mapRows{mapRows},
      m_mapCols{mapCols}
{
    assert(m_mapRows >= 0 && m_mapCols >= 0);
    setWindowTitle(tr("Resize Grid"));
    setWindowModality(Qt::WindowModal);
    createForm();
}

std::pair<int, int> ResizeGridDialog::processDial()
{
    using Rows = int;
    using Cols = int;
    std::pair<Rows, Cols> values{0, 0};

    QLabel msg{};

    do{
        exec();
        if(!validInput() && msg.text().isEmpty()){
            msg.setText("Cannot reduce more than grid's size !");
            m_form.addRow(&msg);
        }
    } while(!validInput() && result() == QDialog::Accepted);

    if(result() == QDialog::Accepted){
        values = std::make_pair(m_spinRow->value(), m_spinCol->value());
    }

    return values;
}

void ResizeGridDialog::createSpinBox()
{
    m_spinRow = new QSpinBox{this};
    m_spinRow->setRange(-99, 99);
    m_spinRow->selectAll();
    QLabel *labelRow = new QLabel{tr("Row(s)"), this};
    QHBoxLayout *layoutRow = new QHBoxLayout{};
    layoutRow->addWidget(labelRow);
    layoutRow->addWidget(m_spinRow);

    m_spinCol = new QSpinBox{this};
    m_spinCol->setRange(-99, 99);
    QLabel *labelCol = new QLabel{tr("Col(s)"), this};
    QHBoxLayout *layoutCol = new QHBoxLayout{};
    layoutCol->addWidget(labelCol);
    layoutCol->addWidget(m_spinCol);

    QVBoxLayout *vLayout = new QVBoxLayout{};
    vLayout->addLayout(layoutRow);
    vLayout->addLayout(layoutCol);

    m_groupBox = new QGroupBox{"How many ?"};
    m_groupBox->setLayout(vLayout);
}

void ResizeGridDialog::createForm()
{
    QLabel *infos = new QLabel{"Note :\n"
                               " - positive value expand\n"
                               " - negative value reduce"};
    m_form.addRow(infos);

    createSpinBox();
    m_form.addRow(m_groupBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, this};
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    m_form.addRow(buttonBox);
}

bool ResizeGridDialog::validInput() const
{
    bool good{true};

    const int rows = m_spinRow->value();
    const int cols = m_spinCol->value();
    if(cols < 0 && abs(cols) >= m_mapCols) good = false;
    if(rows < 0 && abs(rows) >= m_mapRows) good = false;

    return good;
}
