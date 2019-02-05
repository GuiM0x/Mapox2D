#include "filltilecommand.h"

FillTileCommand::FillTileCommand(MapScene *mapScene, QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    m_textureInList = m_mapScene->currentTextureName();
    m_oldTextureName = m_mapScene->currentTileName();
    m_tileIndex = m_mapScene->currentTile();
    QString infos = "Tile [" + QString::number(m_tileIndex) +
                    "] texture [" + m_textureInList + "] added";
    setText(infos);
}

void FillTileCommand::undo()
{
    assert(m_mapScene != nullptr && "FillTileCommand::m_mapScene cannot be null");
    m_mapScene->fillTile(m_tileIndex, m_oldTextureName, true);
}

void FillTileCommand::redo()
{
    assert(m_mapScene != nullptr && "FillTileCommand::m_mapScene cannot be null");
    m_mapScene->fillTile(m_tileIndex, m_textureInList);
}
