#include "stylesheetregister.h"

StyleSheetRegister::StyleSheetRegister(const QString& styleSheetFolderName)
{
    assert(!styleSheetFolderName.isEmpty());
    m_folderName += QString{styleSheetFolderName + "/"};
}

void StyleSheetRegister::registerStyleSheet(const QString& fileName, StyleSheet style)
{
    if(style == StyleSheet::NoStyle)
        return;
    assert(!fileName.isEmpty());
    const QString filePath = m_folderName + fileName;
    QFile file{filePath};
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        throw(std::runtime_error{"Unable to open file : " + filePath.toStdString()});
    }
    QTextStream in{&file};
    QString fileToString{};
    while(!in.atEnd()){
        fileToString += in.readLine();
    }
    m_styleRegistered[style] = fileToString;
}

QString StyleSheetRegister::operator[](StyleSheet style) const
{
    auto it = m_styleRegistered.find(style);
    assert(it != std::end(m_styleRegistered) && "Style not registered.");
    QString styleString{it->second};
    return styleString;
}
