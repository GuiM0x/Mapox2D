#include "pastecommand.h"

PasteCommand::PasteCommand(MapScene* scene,
                           const QList<TileItem*>& copiedTiles,
                           QList<TileItem*> *pastedTiles,
                           QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = scene;
    m_copiedTiles = copiedTiles;
    m_pastedTiles = pastedTiles;
    assert(m_mapScene != nullptr && m_pastedTiles != nullptr);
    QString infos = "Selection Pasted";
    setText(infos);
}

void PasteCommand::undo()
{
    for(const auto& item : m_copiedTiles){
        m_mapScene->removeItem(item);
    }
    m_pastedTiles->clear();
    emit m_mapScene->triggerTool();
}

void PasteCommand::redo()
{
    m_pastedTiles->clear();
    *m_pastedTiles = m_copiedTiles;
    for(const auto& tileCopied : *m_pastedTiles){
        m_mapScene->addItem(tileCopied);
    }
}
