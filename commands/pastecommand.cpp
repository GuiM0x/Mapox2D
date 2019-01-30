#include "pastecommand.h"

PasteCommand::PasteCommand(MapScene* scene,
                           QList<TileItem*> *copiedTiles,
                           QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = scene;
    m_copiedTiles = copiedTiles;
    m_tmpSavedTiles = *m_copiedTiles;
    assert((m_mapScene != nullptr && m_copiedTiles != nullptr) &&
           "m_mapScene or m_copiedTiles cannot be null");
    QString infos = "Selection Pasted";
    setText(infos);
}

void PasteCommand::undo()
{
    for(const auto& item : *m_copiedTiles){
        m_mapScene->removeItem(item);
    }
    m_copiedTiles->clear();
}

void PasteCommand::redo()
{
    if(m_copiedTiles->empty()){
        *m_copiedTiles = m_tmpSavedTiles;
    }
    for(const auto& tileCopied : *m_copiedTiles){
        m_mapScene->addItem(tileCopied);
    }
}
