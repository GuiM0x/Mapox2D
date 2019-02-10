#ifndef UTILITYTOOLS_H
#define UTILITYTOOLS_H

#include "tileitem.h"

#include <random>
#include <ctime>

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

    inline
    int rollTheDice(int min, int max)
    {
        static std::mt19937 g{static_cast<unsigned>(std::time(nullptr))};
        std::uniform_int_distribution<> dist{min, max};
        return dist(g);
    }

    inline
    QString createRandomKey(std::size_t keySize)
    {
        QString key{};
        for(std::size_t i = 0; i < keySize; ++i){
            QString caracter{};
            if(rollTheDice(0,1) == 0){
                // Go to random alpha
                if(rollTheDice(0, 1) == 0){
                    // Go to uppercase
                    caracter = static_cast<char>(rollTheDice(65, 90));
                } else {
                    // Go to lowercase
                    caracter = static_cast<char>(rollTheDice(97, 122));
                }
            } else {
                // Go to random digit
                caracter = QString::number(rollTheDice(0, 9));
            }
            key += caracter;
        }
        return key;
    }
}

#endif // UTILITYTOOLS_H
