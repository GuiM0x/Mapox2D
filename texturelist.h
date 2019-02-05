#ifndef TEXTURELIST_H
#define TEXTURELIST_H

#include "tools/stringtools.h"

#include <QDebug>
#include <QListWidget>
#include <QStringList>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QMenu>
#include <QInputDialog>

class TextureList : public QListWidget
{
    Q_OBJECT

public:
    TextureList(QWidget *parent = nullptr);

signals:
    void docModified();
    void renameTileScene(const QString& oldName, const QString& newName);

private slots:
    void renameTriggered();
    void removeFromMapTriggered();
    void removeFromListTriggered();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

public:
    void addTexture(const QString& fileName, bool fromLoadFile = false);
    QListWidgetItem* addTexture(const QBrush& brush, const QString& textureName);
    void addTexture(const QList<QPixmap>& textures, const QString& fileName);
    QPixmap getTexture(const QString& textureName);
    std::map<QString, QPixmap>* textureList();
    void clean();
    bool textureAlreadyExists(const QString& fileName, bool messageBoxActived = true);

private:
    void createContextMenu();
    QListWidgetItem* canShowContextMenu(const QPoint& viewPortPos);
    QString processRenameDialog();
    void replaceTextureNameInList(const QString& newName);

private:
    std::map<QString, QPixmap>  m_textures{};
    QMenu                      *m_contextMenu{nullptr};
    QAction                    *m_renameAct{nullptr};
    QAction                    *m_removeFromMapAct{nullptr};
    QAction                    *m_removeFromListAct{nullptr};
    QListWidgetItem            *m_itemContextMenu{nullptr};
};

#endif // TEXTURELIST_H
