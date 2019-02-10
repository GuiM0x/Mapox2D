#include "filltilecommand.h"

FillTileCommand::FillTileCommand(MapScene *mapScene, QUndoCommand *parent)
    : QUndoCommand{parent}
{
    m_mapScene = mapScene;
    assert(m_mapScene != nullptr);

    m_textureInList = m_mapScene->currentTextureName();
    m_tileIndex     = m_mapScene->currentTile();

    QString infos = "Tile [" + QString::number(m_tileIndex) +
                    "] texture [" + m_textureInList + "] added";
    setText(infos);
}

void FillTileCommand::undo()
{
    m_mapScene->removeLastLayer(m_tileIndex);
}

void FillTileCommand::redo()
{
    m_mapScene->fillTile(m_tileIndex, m_textureInList);
}
