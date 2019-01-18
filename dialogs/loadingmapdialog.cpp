#include "loadingmapdialog.h"

LoadingMapDialog::LoadingMapDialog(int totalTiles,
                                   const QString& labelText,
                                   const QString& cancelButton,
                                   int minPercent,
                                   int maxPercent,
                                   QWidget *parent,
                                   Qt::WindowFlags f)
    : QProgressDialog{labelText, cancelButton, minPercent, maxPercent, parent, f}
{
    m_totalTiles = totalTiles;
    setWindowModality(Qt::WindowModal);
    setMinimumDuration(0);
    setValue(0);
    show();
}

void LoadingMapDialog::updateBar(int currentIndex)
{
    // If 1% of totalTiles >= 1 tile
    if(m_totalTiles/100.f >= 1){
        // Check if current index >= 1% of total * current step
        if(currentIndex >= (m_totalTiles/100.f)*m_steps+1){
            // 1 step = 1%
            ++m_steps;
            setValue(m_steps);
        }
    } else {
        // Display % relative to current tile
        setValue(static_cast<int>(currentIndex/(m_totalTiles/100.f)));
    }
}
