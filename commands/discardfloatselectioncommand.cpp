#include "discardfloatselectioncommand.h"

DiscardFloatSelectionCommand::DiscardFloatSelectionCommand(MapScene *mapScene,
                                                           const QList<TileItem*> *floatSelection,
                                                           QUndoCommand *parent)
    : QUndoCommand{parent},
      m_mapScene{mapScene},
      m_floatSelectionFromView{floatSelection}
{
    assert(m_mapScene != nullptr && m_floatSelectionFromView != nullptr);
    QString infos = "Float Selection discarded\ndiscard float selection";
    setText(infos);
}

void DiscardFloatSelectionCommand::undo()
{
    for(const auto& item : *m_floatSelectionFromView){
        m_mapScene->addItem(item);
    }
    emit m_mapScene->triggerTool(true, ToolType::MoveSelection);
}

void DiscardFloatSelectionCommand::redo()
{
    for(const auto& item : *m_floatSelectionFromView){
        m_mapScene->removeItem(item);
    }
}
