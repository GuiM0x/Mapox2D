#ifndef TEXTURELIST_H
#define TEXTURELIST_H

#include "tools/stringtools.h"

#include <QDebug>
#include <QListWidget>
#include <QStringList>
#include <QMessageBox>

class TextureList : public QListWidget
{
    Q_OBJECT

public:
    TextureList(QWidget *parent = nullptr);

signals:
    void docModified();

public:
    void addTexture(const QString& fileName, bool fromLoadFile = false);
    void addTexture(const QList<QPixmap>& textures, const QString& fileName);
    QPixmap getTexture(const QString& textureName);
    std::map<QString, QPixmap>* textureList();
    void clean();

private:
    bool textureAlreadyExists(const QString& fileName);

private:
    std::map<QString, QPixmap> m_textures{};
};

#endif // TEXTURELIST_H
