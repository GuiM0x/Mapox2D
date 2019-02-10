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
    // Correponding to the order that datas have been
    // pushed by DataSaver
    enum class DataIndex{
        ProjectName,
        TexturesListPath,
        TileWidth,
        TileHeight,
        Rows,
        Cols,
        Tiles
    };

    struct Tile{
        int     index{};
        QString name{};
        QString path{};
    };

public:
    DataLoader(TextureList *textureList, MapScene *mapScene);

    void loadData(const QString& fileName);

private:
    void loadFromFile(const QString& fileName);
    void createNewTextureList();
    void createNewMap();

    QString extractFromDelimiter(const QString& s, QChar delimiter);
    std::vector<QString> splitFromDelimiter(const QString& s, QChar delimiter);

private:
    TextureList          *m_textureList{nullptr};
    MapScene             *m_mapScene{nullptr};
    std::vector<QString>  m_datas{};
};

#endif // DATALOADER_H
