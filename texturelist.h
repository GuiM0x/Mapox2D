#ifndef TEXTURELIST_H
#define TEXTURELIST_H

#include <QDebug>
#include <QListWidget>
#include <QStringList>

class TextureList : public QListWidget
{
    Q_OBJECT

public:
    TextureList(QWidget *parent = nullptr);

public:
    void addTexture(const QString& fileName);
    QPixmap getTexture(const QString& textureName);

private:
    QString cutFileName(const QString& fileName) const;

private:
    std::map<QString, QPixmap> m_textures{};
    std::map<QString, QString> m_texturesPaths{};
};

#endif // TEXTURELIST_H
