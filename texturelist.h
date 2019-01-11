#ifndef TEXTURELIST_H
#define TEXTURELIST_H

#include <QDebug>
#include <QListWidget>

class TextureList : public QListWidget
{
    Q_OBJECT

public:
    TextureList(QWidget *parent = nullptr);

private slots:
    void currentActiveItem(QListWidgetItem* item);

public:
    void addTexture(const QString& fileName);
};

#endif // TEXTURELIST_H
