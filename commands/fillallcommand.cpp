#include "fillallcommand.h"

FillAllCommand::FillAllCommand(MapScene *mapScene,
                               const QString& textureName,
                               QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_textureName = textureName;
    QString infos = "Grid filled with [" + m_textureName + "]";
    setText(infos);
}

void FillAllCommand::undo()
{
    assert(m_mapScene != nullptr && "FillAllCommand::m_mapScene cannot be null");
    m_mapScene->fillAll(m_oldTilesTexturesNames);
}

void FillAllCommand::redo()
{
    assert(m_mapScene != nullptr && "FillAllCommand::m_mapScene cannot be null");
    m_oldTilesTexturesNames = m_mapScene->fillAll(m_textureName);
}
