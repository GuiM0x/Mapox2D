#include "discardfloatselectioncommand.h"

DiscardFloatSelectionCommand::DiscardFloatSelectionCommand(MapScene *mapScene,
                                                           QList<TileItem*> *floatSelection,
                                                           QUndoCommand *parent)
    : QUndoCommand{parent},
      m_mapScene{mapScene},
      m_floatSelectionFromView{floatSelection}
{
    assert(m_mapScene != nullptr && m_floatSelectionFromView != nullptr);

    QString infos = "Float Selection discarded\ndiscard float selection";
    setText(infos);

    for(const auto& item : *m_floatSelectionFromView){
        // Selection copied
        m_copiedSelection.push_back(TileItem::copy(item));
    }
}

void DiscardFloatSelectionCommand::undo()
{
    // Retrieve float selection
    m_floatSelectionFromView->clear();
    for(const auto& copy : m_copiedSelection){
        m_floatSelectionFromView->push_back(copy);
        m_mapScene->addItem(copy);
    }

    m_mapScene->triggerTool(true, ToolType::MoveSelection);
}

void DiscardFloatSelectionCommand::redo()
{
    // Clear float selection
    for(const auto& item : *m_floatSelectionFromView){
        if(item->scene() != nullptr)
            m_mapScene->removeItem(item);
    }
    m_floatSelectionFromView->clear();
}
