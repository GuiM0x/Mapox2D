#include "datasaver.h"

DataSaver::DataSaver(MapScene *mapScene, TextureList *textureList)
    : m_mapScene{mapScene}, m_textureList{textureList}
{
    assert(m_mapScene != nullptr && m_textureList != nullptr);
    m_rows       = m_mapScene->rows();
    m_cols       = m_mapScene->cols();
    m_tileWidth  = m_mapScene->tileWidth();
    m_tileHeight = m_mapScene->tileHeight();
}

bool DataSaver::saveAllDatas(const QString& fileName)
{
    // fileName is the path selected by user to save .m2x
    m_fullPathSaveFile = fileName;
    const QString cutFromPath = StringTools::cutFileName(m_fullPathSaveFile);
    const QString projectName = StringTools::cutExtensionFileName(cutFromPath);
    m_folderProjectName = projectName;
    m_datas.push_back("projectName=" + projectName);

    textureListToData();
    gridToData();

    return dataToFile();
}

void DataSaver::textureListToData()
{
    QDir path = createAppDataFolder(m_folderProjectName + "/texturelist/");

    QString tmp = "texturesInListPath=";

    QList<QListWidgetItem*> widgetItems = m_textureList->widgetItems();
    if(!widgetItems.isEmpty()){
        for(const auto& item : widgetItems){
            const QString textureName{item->toolTip()};
            QPixmap texture = m_textureList->getTexture(textureName);
            // Correct a bug when no textures was in list and empty name file was created
            if(!texture.isNull()){
                QString newFileName = path.path() + "/" + textureName + ".dat";
                QFile file{newFileName};
                file.open(QIODevice::WriteOnly);
                texture.save(&file, "PNG");
                tmp += (newFileName + "#");
            }
        }
    }

    m_datas.push_back(tmp);
}

void DataSaver::gridToData()
{
    const QDir path = createAppDataFolder(m_folderProjectName + "/maptextures/");
    const std::vector<TileItem*> *allTiles = m_mapScene->tiles();

    m_datas.push_back("tileWidth="  + QString::number(m_tileWidth));
    m_datas.push_back("tileHeight=" + QString::number(m_tileHeight));
    m_datas.push_back("rows="       + QString::number(m_rows));
    m_datas.push_back("cols="       + QString::number(m_cols));

    for(const auto& tile : *allTiles){
        const int           index = tile->index();
        const QString textureName = tile->name();
        const QPixmap     texture = tile->brush().texture();
        if(!textureName.isEmpty()){
            QString hashTextureName = UtilityTools::createRandomKey(20);
            QString newFileName = path.path() + "/" + hashTextureName + ".dat";
            QFile file{newFileName};
            file.open(QIODevice::WriteOnly);
            texture.save(&file, "PNG");
            const QString data = QString::number(index) + "#"
                                 + textureName + "#"
                                 + newFileName;
            m_datas.push_back(data);
        }
    }
}

bool DataSaver::dataToFile() const
{
    QFile file{m_fullPathSaveFile};
    if(!file.open(QFile::WriteOnly)){
        QMessageBox::warning(nullptr, QObject::tr("Application"),
                                      QObject::tr("Cannot write file %1:\n%2.")
                                      .arg(QDir::toNativeSeparators(m_fullPathSaveFile),
                                      file.errorString()));
        return false;
    }

    // Prepare to write in file
    QDataStream out{&file};
    for(const auto& data : m_datas){
        out << QString(data); // implicit break line
    } 
    return true;
}

QDir DataSaver::createAppDataFolder(const QString& name)
{
    QDir path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + name;
    if(!path.exists()){
        path.mkpath(path.path());
    } else {
        path.removeRecursively();
        path.mkpath(path.path());
    }

    return path;
}

