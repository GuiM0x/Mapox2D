#include "datasaver.h"

DataSaver::DataSaver(MapScene *mapScene, TextureList *textureList)
    : m_mapScene{mapScene}, m_textureList{textureList}
{
    assert(m_mapScene != nullptr && m_textureList != nullptr);
}

bool DataSaver::saveAllDatas(const QString& fileName)
{
    //qDebug() << "File path selected : " << fileName;
    textureToAppData(fileName);
    matrixToData();
    if(dataToFile())
        return true;
    return false;
}

void DataSaver::textureToAppData(const QString& fileName)
{
    // fileName is the path selected by user to save .m2x
    // - cut the name to create folder with the project name
    // - Then add to path to create the folder
    m_fullPathSaveFile = fileName;

    const QString cutFromPath = StringTools::cutFileName(fileName);
    const QString projectName = StringTools::cutExtensionFileName(cutFromPath);
    m_folderProjectName = projectName;

    QDir path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
            "/" + projectName + "/textures/";
    if(!path.exists()){
        path.mkpath(path.path());
    } else {
        path.removeRecursively();
        path.mkpath(path.path());
    }

    std::map<QString, QPixmap> *textureMap = m_textureList->textureList();
    if(!textureMap->empty()){
        for(auto it = std::begin(*textureMap); it != std::end(*textureMap); ++it){
            QPixmap texture = it->second;
            // Correct a bug when no textures was in list and empty name file was created
            if(!texture.isNull()){
                QString newFileName = path.path() + "/" + it->first + ".dat";
                m_texturesNewPath.push_back(newFileName);
                QFile file{newFileName};
                file.open(QIODevice::WriteOnly);
                texture.save(&file, "PNG");
                //qDebug() << "File saved : " << newFileName;
            }
        }
    }
}

void DataSaver::matrixToData()
{
    int rows = m_mapScene->rows();
    int cols = m_mapScene->cols();
    int tileWidth = m_mapScene->tileWidth();
    int tileHeight = m_mapScene->tileHeight();

    m_matrixDatas.push_back("tileWidth=");
    m_matrixDatas.push_back(QString::number(tileWidth));
    m_matrixDatas.push_back("tileHeight=");
    m_matrixDatas.push_back(QString::number(tileHeight));
    m_matrixDatas.push_back("rows=");
    m_matrixDatas.push_back(QString::number(rows));
    m_matrixDatas.push_back("cols=");
    m_matrixDatas.push_back(QString::number(cols));
    m_matrixDatas.push_back("matrix=");

    std::vector<QString> *allTilesName = m_mapScene->allTilesName();
    for(std::size_t i = 0; i < allTilesName->size(); ++i){
        const QString textureName = (*allTilesName)[i];
        if(!textureName.isEmpty()){
            m_matrixDatas.push_back(QString::number(i));
            m_matrixDatas.push_back(textureName);
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
    out << QString("project="); // implicit break line
    out << QString(m_folderProjectName); // implicit break line
    out << QString("texturesPath="); // implicit break line
    for(const auto& x : m_texturesNewPath){
        out << QString(x); // implicit break line
    }
    for(const auto& x : m_matrixDatas){
        out << QString(x); // implicit break line
    }

    return true;
}

