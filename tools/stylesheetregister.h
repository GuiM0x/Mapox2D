#ifndef STYLESHEETREGISTER_H
#define STYLESHEETREGISTER_H

#include "tools/singleton.h"

#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QString>

enum class StyleSheet{
    NoStyle,
    MenuBar,
    TextureList,
    ToolBar,
    MapView,
    UndoView,
    DockWidget,
    StatusBar,
    Menu
};

class StyleSheetRegister : public Singleton<StyleSheetRegister>
{
    friend class Singleton<StyleSheetRegister>;

private:
    explicit StyleSheetRegister(const QString& styleSheetFolderName);

public:
    void registerStyleSheet(const QString& filename,
                            StyleSheet style = StyleSheet::NoStyle);

public:
    QString operator[](StyleSheet style) const;

private:
    QString m_folderName{":/"};
    std::map<StyleSheet, QString> m_styleRegistered{};
};

#endif // STYLESHEETREGISTER_H
