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

    /*for(const auto& x : m_datas)
        qDebug() << x;*/
}

void DataLoader::createNewTextureList()
{
    m_textureList->clean();

    std::size_t indexBeginTexturePath{};
    std::size_t indexEndTexturePath{};
    for(std::size_t i = 0; i < m_datas.size(); ++i){
        if(m_datas[i] == "texturesPath="){
            indexBeginTexturePath = i+1;
        }
        if(m_datas[i] == "tileWidth="){
            indexEndTexturePath = i;
        }
    }

    if(indexEndTexturePath == 0)
        indexEndTexturePath = m_datas.size();

    for(std::size_t i = indexBeginTexturePath; i < indexEndTexturePath; ++i){
        const QString filePath = m_datas[i];
        QListWidgetItem *item = m_textureList->addTexture(m_datas[i], true);
        if(item != nullptr){
            // Get extension from filePath
            QString ext{};
            int start = filePath.size() - 4;
            assert(start >= 0);
            for(int i = start; i < filePath.size(); ++i){
                ext += filePath[i];
            }
            // If extension == hide -> item hide in Texture List
            if(ext == "hide")
                item->setHidden(true);
        }
    }
}

void DataLoader::createNewMap()
{
    int tileWidth{};
    int tileHeight{};
    int rows{};
    int cols{};
    std::size_t indexBeginMatrix{};
    for(std::size_t i = 0; i < m_datas.size(); ++i){
        if(m_datas[i] == "tileWidth=")
            tileWidth = m_datas[i+1].toInt();
        if(m_datas[i] == "tileHeight=")
            tileHeight = m_datas[i+1].toInt();
        if(m_datas[i] == "rows=")
            rows = m_datas[i+1].toInt();
        if(m_datas[i] == "cols=")
            cols = m_datas[i+1].toInt();
        if(m_datas[i] == "matrix=")
            indexBeginMatrix = i+1;
    }

    m_mapScene->createMatrix(tileWidth, tileHeight, rows, cols);

    std::vector<int> indexOnMatrix{};
    std::vector<QString> texturesName{};
    for(std::size_t i = indexBeginMatrix; i < m_datas.size(); ++i){
        // lambda
        auto checkDigit = [](const std::string& s){
            auto it = std::begin(s);
            while (it != std::end(s) && std::isdigit(*it)) ++it;
                return !s.empty() && it == std::end(s);
        };
        // use of lambda
        if(checkDigit(m_datas[i].toStdString())){
            indexOnMatrix.push_back(m_datas[i].toInt());
        }
        else
            texturesName.push_back(m_datas[i]);
    }
    for(std::size_t i = 0; i < indexOnMatrix.size(); ++i){
        /*qDebug() << "[" + QString::number(indexOnMatrix[i]) + "] " +
                    texturesName[i];*/
        m_mapScene->fillTile(indexOnMatrix[i], texturesName[i]);
    }
}
