#ifndef DATASAVER_H
#define DATASAVER_H

#include "mapscene.h"
#include "texturelist.h"
#include "tools/stringtools.h"

#include <vector>

#include <QDebug>
#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

class DataSaver
{
public:
    DataSaver(MapScene *mapScene, TextureList* textureList);

    bool saveAllDatas(const QString& fileName);

private:
    void textureToAppData(const QString& fileName);
    bool matrixToData() const;

private:
    MapScene            *m_mapScene{nullptr};
    TextureList         *m_textureList{nullptr};
    std::vector<QString> m_texturesNewPath{};
    QString              m_fullPathSaveFile{};
    QString              m_folderProjectName{};
};

#endif // DATASAVER_H
