#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include "texturelist.h"
#include "mapscene.h"
#include "tools/utilitytools.h"

#include <map>
#include <algorithm>

#include <QString>
#include <QDir>

class DataGenerator
{
public:
    DataGenerator(MapScene *mapScene);

public:
    void generateDatas(const QString& path) const;

private:
    std::map<QString, QImage> generateImageFiles(const QString& pathFolder) const;
    QString generateArray(const std::map<QString, QImage>& imageFiles) const;

private:
    MapScene      *m_mapScene{nullptr};
};

#endif // DATAGENERATOR_H
