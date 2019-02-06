#include "removeitemfromtexturelistcommand.h"

RemoveItemFromTextureListCommand::RemoveItemFromTextureListCommand(TextureList *textureList,
                                                                   const QString& textureName,
                                                                   QUndoCommand *parent)
    : QUndoCommand{parent},
      m_textureList{textureList}
{
    assert(m_textureList != nullptr);
    m_itemTaken = m_textureList->widgetItem(textureName);
    assert(m_itemTaken != nullptr);
    QString infos = "Item removed from List\npaste selection";
    setText(infos);
}

void RemoveItemFromTextureListCommand::undo()
{
    m_textureList->addItem(m_itemTaken);
}

void RemoveItemFromTextureListCommand::redo()
{
    m_textureList->takeItem(m_textureList->row(m_itemTaken));
}
