#include "datagenerator.h"

DataGenerator::DataGenerator(MapScene *mapScene)
    : m_mapScene{mapScene}
{
    assert(m_mapScene != nullptr);
}

void DataGenerator::generateDatas(const QString &path) const
{
    const QString newFolderPath = path + "/MapDatas_gen_" + UtilityTools::createRandomKey(5);
    qDebug() << "DataGenerator::generateDatas - New folder path => " << newFolderPath;
    QDir newDir{newFolderPath};
    if(!newDir.exists()){
        newDir.mkpath(newFolderPath);
    } else {
        newDir.removeRecursively();
    }

    // -> generate texture/*.png
    const QString texturesPath = newFolderPath + "/textures";
    newDir.mkpath(texturesPath);
    auto imageFiles = generateImageFiles(texturesPath);

    // -> generate array.txt
    QFile arrayFile{newFolderPath + "/array.txt"};
    if(arrayFile.exists()) arrayFile.remove();
    arrayFile.open(QFile::WriteOnly);
    const QString arrayGenerated = generateArray(imageFiles);
    QTextStream out{&arrayFile};
    out << arrayGenerated;
}

std::map<QString, QImage> DataGenerator::generateImageFiles(const QString& pathFolder) const
{
    const std::size_t rows = static_cast<std::size_t>(m_mapScene->rows());
    const std::size_t cols = static_cast<std::size_t>(m_mapScene->cols());
    const std::vector<TileItem*> *tiles = m_mapScene->tiles();
    assert(tiles->size() == rows*cols);

    std::map<QString, QImage> mapper{};

    // Here, we check all tiles on map
    for(std::size_t i = 0; i < rows; ++i){
        for(std::size_t j = 0; j < cols; ++j){
            const TileItem *tile = (*tiles)[i*cols+j];
            // If mapper doesn't have tile registered yet,
            // we insert the tile's name (key) and the tile's texture (value) in mapper
            if(mapper.find(tile->name()) == std::end(mapper)){
                mapper[tile->name()] = tile->image();
            } else {
                // Else, maybe we've got a new composed Image
                // Yes, because of the layer system on tile, the name of a tile
                // is the last name of layer added.
                // So, we check if the image tile corresponding to the image registered
                // under the current tile's name
                const QImage tileTexture = tile->image();
                auto it = std::find_if(std::begin(mapper), std::end(mapper),
                                       [&](const std::pair<QString, QImage>& p){
                                           return p.second == tileTexture;
                                       });
                // If image not corresponding, we've got a composed image to register in mapper,
                // We ensure to create a new name for the texture, then add it to the mapper
                if(it == std::end(mapper)){
                    const QString genName = tile->name() + "_" + UtilityTools::createRandomKey(4);
                    mapper[genName] = tile->image();
                }
            }
        }
    }

    // Create *.png files
    for(const auto& it : mapper){
        const QString filePath = pathFolder + "/" + it.first + ".png";
        if(!it.second.isNull()){
            QFile file{filePath};
            file.open(QIODevice::WriteOnly);
            it.second.save(&file, "PNG");
        }
    }

    return mapper;
}

QString DataGenerator::generateArray(const std::map<QString, QImage>& imageFiles) const
{
    const std::map<QString, QImage> mapperImages = imageFiles;
    std::map<QString, int>          mapperId{};

    // Create mapper for associate ID to texture's name
    int id{0};
    for(const auto& it : mapperImages){
        mapperId[it.first] = id;
        ++id;
    }

    const std::size_t rows = static_cast<std::size_t>(m_mapScene->rows());
    const std::size_t cols = static_cast<std::size_t>(m_mapScene->cols());
    const std::vector<TileItem*> *tiles = m_mapScene->tiles();
    assert(tiles->size() == rows*cols);

    QString datas{};

    for(const auto& it : mapperId){
        datas += QString::number(it.second) + " = " + it.first + "\n";
    }

    datas += "\n";

    // Here, we check all tiles on map
    for(std::size_t i = 0; i < rows; ++i){
        for(std::size_t j = 0; j < cols; ++j){
            const TileItem *tile = (*tiles)[i*cols+j];
            const QImage tileTexture = tile->image();
            // Find image corresponding in mapperImages
            auto it = std::find_if(std::begin(mapperImages), std::end(mapperImages),
                                   [&](const std::pair<QString, QImage>& p){
                                       return p.second == tileTexture;
                                   });
            assert(it != std::end(mapperImages));
            // Then, we use the name to get ID from mapper ID
            datas += QString::number(mapperId[it->first]) + ", ";
        }
        datas += '\n';
    }

    return datas;
}
