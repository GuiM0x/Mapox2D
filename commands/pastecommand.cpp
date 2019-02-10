#include "pastecommand.h"

PasteCommand::PasteCommand(MapScene *scene,
                           const QList<TileItem*>& copiedTiles,
                           QList<TileItem*> *floatSelection,
                           QUndoCommand *parent)
    : QUndoCommand{parent},
      m_mapScene{scene},
      m_floatSelectionFromView{floatSelection}
{
    assert(m_mapScene != nullptr &&
            m_floatSelectionFromView != nullptr);

    QString infos = "Selection Pasted\npaste selection";
    setText(infos);

    for(const auto& item : copiedTiles){
        m_copy.push_back(TileItem::copy(item));
    }
}

void PasteCommand::undo()
{
    // Clear float Selection
    for(const auto& item : *m_floatSelectionFromView){
        if(item->scene() != nullptr)
            m_mapScene->removeItem(item);
    }
    m_floatSelectionFromView->clear();

    m_mapScene->triggerTool(true, ToolType::NoTool);
}

void PasteCommand::redo()
{
    // Create Float selection
    m_floatSelectionFromView->clear();
    for(const auto& copy : m_copy){
        m_floatSelectionFromView->push_back(copy);
        m_mapScene->addItem(copy);
    }

    m_mapScene->triggerTool(true, ToolType::MoveSelection);
}
