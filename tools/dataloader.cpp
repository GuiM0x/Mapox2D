#include "dataloader.h"

DataLoader::DataLoader(TextureList *textureList, MapScene *mapScene)
    : m_textureList{textureList}, m_mapScene{mapScene}
{

}

void DataLoader::loadData(const QString &fileName)
{
    loadFromFile(fileName);
    createNewTextureList();
    createNewMap();
}

void DataLoader::loadFromFile(const QString &fileName)
{
    // Open file in ReadOnly Mode
    QFile file{fileName};
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::warning(nullptr, QObject::tr("Application"),
                                     QObject::tr("Cannot read file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }
    // Prepare to read in file
    QDataStream in{&file};
    while(!in.atEnd()){
        QString tmp{};
        in >> tmp; // insert current data to string
        m_datas.push_back(tmp); // save data in vector
    };
}

void DataLoader::createNewTextureList()
{
    m_textureList->clean();

    std::size_t index = static_cast<std::size_t>(DataIndex::TexturesListPath);
    QString data = extractFromDelimiter(m_datas[index], '=');
    std::vector<QString> texturesPath = splitFromDelimiter(data, '#');
    for(const auto& path : texturesPath){
        m_textureList->addTexture(path, true);
    }
}

void DataLoader::createNewMap()
{
    int tileWidth  = extractFromDelimiter(m_datas[static_cast<std::size_t>(DataIndex::TileWidth)] , '=').toInt();
    int tileHeight = extractFromDelimiter(m_datas[static_cast<std::size_t>(DataIndex::TileHeight)], '=').toInt();
    int rows       = extractFromDelimiter(m_datas[static_cast<std::size_t>(DataIndex::Rows)]      , '=').toInt();
    int cols       = extractFromDelimiter(m_datas[static_cast<std::size_t>(DataIndex::Cols)]      , '=').toInt();

    m_mapScene->createMatrix(tileWidth, tileHeight, rows, cols);

    std::size_t indexTilesData = static_cast<std::size_t>(DataIndex::Tiles);
    std::vector<DataLoader::Tile> tiles{};
    for(std::size_t i = indexTilesData; i < m_datas.size(); ++i){
        const QString data{m_datas[i]};
        const std::vector<QString> cutted = splitFromDelimiter(data, '#');
        DataLoader::Tile tile{cutted[0].toInt(), // index
                              cutted[1],         // name
                              cutted[2]};        // path
        tiles.push_back(tile);
    }

    for(const auto& tile : tiles){
        const int     index        = tile.index;
        const QString textureName  = tile.name;
        const QPixmap texture(tile.path);
        TileItem *item = m_mapScene->itemByIndex(index);
        item->addLayer(textureName, QBrush{texture});
    }
}

QString DataLoader::extractFromDelimiter(const QString& s, QChar delimiter)
{
    QString extracted{};
        bool extract{false};
        for(const auto& it : s){
            if(extract){
                extracted += it;
            }
            if(it == delimiter){
                extract = true;
            }
        }
        return extracted;
}

std::vector<QString> DataLoader::splitFromDelimiter(const QString& s, QChar delimiter)
{
    std::vector<QString> datas{};
        QString extracted{};
        for(const auto& it : s){
            if(it != delimiter){
                extracted += it;
            } else {
                datas.push_back(extracted);
                extracted = "";
            }
        }
        if(extracted != datas.back() && !extracted.isEmpty())
            datas.push_back(extracted);
        return datas;
}
