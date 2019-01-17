#include "datasaver.h"

DataSaver::DataSaver(MapScene *mapScene, TextureList *textureList)
    : m_mapScene{mapScene}, m_textureList{textureList}
{

}

bool DataSaver::saveAllDatas(const QString& fileName)
{
    //qDebug() << "File path selected : " << fileName;
    textureToAppData(fileName);
    if(matrixToData())
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
                QString newFileName = path.path() + "/" + StringTools::cutExtensionFileName(it->first) + ".dat";
                m_texturesNewPath.push_back(newFileName);
                QFile file{newFileName};
                file.open(QIODevice::WriteOnly);
                texture.save(&file, "PNG");
                //qDebug() << "File saved : " << newFileName;
            }
        }
    }
}

bool DataSaver::matrixToData() const
{
    QFile file{m_fullPathSaveFile};
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(nullptr, QObject::tr("Application"),
                                      QObject::tr("Cannot write file %1:\n%2.")
                                      .arg(QDir::toNativeSeparators(m_fullPathSaveFile),
                                      file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << "project=" + m_folderProjectName + "\n";
    out << "texturesPath=";
    for(const auto& x : m_texturesNewPath){
        out << x << ';';
    }

    return true;
}

