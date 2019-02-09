#include "pastecommand.h"

PasteCommand::PasteCommand(MapScene *scene,
                           const QList<TileItem*> *copiedTiles,
                           QList<TileItem*> *floatSelection,
                           QUndoCommand *parent)
    : QUndoCommand{parent},
      m_mapScene{scene},
      m_floatSelectionFromView{floatSelection}
{
    assert(m_mapScene != nullptr &&
            m_floatSelectionFromView != nullptr &&
            copiedTiles != nullptr);

    // New item are created to get independant from copiedTiles
    for(const auto& item : *copiedTiles){
        TileItem *tile = TileItem::copy(item);
        m_floatSelectionSaved.push_back(tile);
    }

    QString infos = "Selection Pasted\npaste selection";
    setText(infos);
}

void PasteCommand::undo()
{
    for(const auto& item : *m_floatSelectionFromView){
        m_mapScene->removeItem(item);
    }
    m_floatSelectionFromView->clear();
    m_mapScene->triggerTool(true, ToolType::NoTool);
}

void PasteCommand::redo()
{
    m_floatSelectionFromView->clear();
    for(const auto& item : m_floatSelectionSaved){
        m_floatSelectionFromView->push_back(item);
        m_mapScene->addItem(item);
    }
    m_mapScene->triggerTool(true, ToolType::MoveSelection);
}
