#ifndef DATASAVER_H
#define DATASAVER_H

#include "mapscene.h"
#include "texturelist.h"
#include "tools/stringtools.h"

#include <vector>

#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>

class DataSaver
{
public:
    DataSaver(MapScene *mapScene, TextureList* textureList);

    bool saveAllDatas(const QString& fileName);

private:
    void textureListToData();
    void gridToData();
    bool dataToFile() const;
    QDir createAppDataFolder(const QString& name);

private:
    MapScene             *m_mapScene{nullptr};
    TextureList          *m_textureList{nullptr};
    QString               m_fullPathSaveFile{};
    QString               m_folderProjectName{};
    int                   m_rows{};
    int                   m_cols{};
    int                   m_tileWidth{};
    int                   m_tileHeight{};

    std::vector<QString>  m_datas{};
};

#endif // DATASAVER_H
