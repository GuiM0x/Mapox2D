#include "newmapdialog.h"

NewMapDialog::NewMapDialog(QWidget *parent)
    : QDialog{parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint}
{
    setWindowTitle(tr("Create a new map"));
    createForm();
}

std::map<QString, int> NewMapDialog::processDial()
{
    QLabel *invalidFieldInfo = new QLabel{this};
    // Exec dialog while input are not valid except if dial is canceled
    do{
        exec();
        if(!validInput() && result() == QDialog::Accepted){
            if(invalidFieldInfo->text().isEmpty()){
                invalidFieldInfo->setText(tr("A field cannot be empty !"));
                m_form.addRow(invalidFieldInfo);
            }
        }
    }while(!validInput() && result() == QDialog::Accepted);


    std::map<QString, int> values{};
    // Ok, we can process if all input are good and user say Ok
    if(result() == QDialog::Accepted){
        for(auto it = std::begin(m_fields); it != std::end(m_fields); ++it){
            values[it->first] = it->second->text().toInt();
        }
    }
    return values;
}

void NewMapDialog::textHasBeenEdited(const QString& text)
{
    if(text.toInt() > m_validatorCount.top()){
        if(m_fields["totalCols"]->text() == text)
            m_fields["totalCols"]->setText(QString::number(m_validatorCount.top()));
        else if(m_fields["totalRows"]->text() == text)
            m_fields["totalRows"]->setText(QString::number(m_validatorCount.top()));
    }
}

void NewMapDialog::createForm()
{
    QLineEdit *lineEditTileWidth = new QLineEdit{this};
    lineEditTileWidth->setValidator(&m_validatorSizeTile);
    m_form.addRow(QString{tr("Tile Width : ")}, lineEditTileWidth);
    m_fields["tileWidth"] = lineEditTileWidth;

    QLineEdit *lineEditTileHeight = new QLineEdit{this};
    lineEditTileHeight->setValidator(&m_validatorSizeTile);
    m_form.addRow(QString{tr("Tile Height : ")}, lineEditTileHeight);
    m_fields["tileHeight"] = lineEditTileHeight;

    QLineEdit *lineEditTotalRows = new QLineEdit{this};
    lineEditTotalRows->setValidator(&m_validatorCount);
    m_form.addRow(QString{tr("Total  Rows : ")}, lineEditTotalRows);
    m_fields["totalRows"] = lineEditTotalRows;
    connect(lineEditTotalRows, &QLineEdit::textEdited, this, &NewMapDialog::textHasBeenEdited);

    QLineEdit *lineEditTotalCols = new QLineEdit{this};
    lineEditTotalCols->setValidator(&m_validatorCount);
    m_form.addRow(QString{tr("Total  Cols : ")}, lineEditTotalCols);
    m_fields["totalCols"] = lineEditTotalCols;
    connect(lineEditTotalCols, &QLineEdit::textEdited, this, &NewMapDialog::textHasBeenEdited);

    QDialogButtonBox *buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, this};
    m_form.addRow(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool NewMapDialog::validInput()
{
    bool valid{true};
    for(auto it = std::begin(m_fields); it != std::end(m_fields); ++it){
        if(it->second->text().isEmpty()){
            valid = false;
            break;
        }
    }
    return valid;
}
