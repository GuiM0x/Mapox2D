#ifndef UTILITYTOOLS_H
#define UTILITYTOOLS_H

#include "tileitem.h"

#include <QDebug>
#include <QPointF>
#include <QSize>
#include <QPen>
#include <QBrush>

enum class ToolType{
    NoTool,
    Selection,
    MoveSelection,
    Brush
};

namespace UtilityTools
{
    inline
    int indexRelativeToPos(const QPointF& pos, const QSize& tileSize, int rows, int cols)
    {
        int index{-1};
        if(pos.x() < 0 ||
                pos.y() < 0 ||
                pos.x() >= cols * tileSize.width() ||
                pos.y() >= rows * tileSize.height()){
            return index;
        }
        else {
            int i = static_cast<int>(pos.y() / tileSize.height());
            int j = static_cast<int>(pos.x() / tileSize.width());
            index = (i * cols) + j;
        }
        return index;
    }
}

#endif // UTILITYTOOLS_H
