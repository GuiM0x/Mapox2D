#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <QString>

namespace StringTools {

/////////////////
// Extract File Name from path
inline QString cutFileName(const QString& fileName)
{
    QString cutted{};
    int index{};
    for(int i = fileName.length(); i > 0; --i){
        if(fileName[i-1] == '/'){
            index = i;
            break;
        }
    }
    for(int i = index; i < fileName.size(); ++i){
        cutted += fileName[i];
    }
    return cutted;
}

/////////////////
// Cut the dot
inline QString cutExtensionFileName(const QString& fileName)
{
    QString cutted{};
    int pointIndex{0};
    for(int i = 0; i < fileName.length(); ++i){
        if(fileName[i] == '.'){
            pointIndex = i;
        }
    }
    if(pointIndex == 0){
        cutted = fileName;
    } else {
        for(int i = 0; i < pointIndex; ++i){
            cutted += fileName[i];
        }
    }

    return cutted;
}

}

#endif // STRINGTOOLS_H
