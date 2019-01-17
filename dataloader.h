#ifndef DATALOADER_H
#define DATALOADER_H

#include "texturelist.h"
#include "mapscene.h"

#include <QDebug>
#include <QObject>
#include <QDir>
#include <QMessageBox>

class DataLoader
{
public:
    DataLoader(TextureList *textureList, MapScene *mapScene);

    void loadData(const QString& fileName);

private:
    void loadFromFile(const QString& fileName);
    void createNewTextureList();
    void createNewMap();

private:
    TextureList          *m_textureList{nullptr};
    MapScene             *m_mapScene{nullptr};
    std::vector<QString>  m_datas{};
};

#endif // DATALOADER_H
