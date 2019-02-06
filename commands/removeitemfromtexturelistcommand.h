#ifndef REMOVEITEMFROMTEXTURELISTCOMMAND_H
#define REMOVEITEMFROMTEXTURELISTCOMMAND_H

#include "texturelist.h"

#include <QUndoCommand>

class RemoveItemFromTextureListCommand : public QUndoCommand
{
public:
    RemoveItemFromTextureListCommand(TextureList *textureList,
                                     const QString& textureName,
                                     QUndoCommand *parent = nullptr);
public:
    void undo() override;
    void redo() override;

private:
    TextureList     *m_textureList{nullptr};
    QListWidgetItem *m_itemTaken{nullptr};
};

#endif // REMOVEITEMFROMTEXTURELISTCOMMAND_H
