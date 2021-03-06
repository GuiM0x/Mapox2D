#ifndef DISCARDFLOATSELECTIONCOMMAND_H
#define DISCARDFLOATSELECTIONCOMMAND_H

#include "mapscene.h"

#include <QUndoCommand>

class DiscardFloatSelectionCommand : public QUndoCommand
{
public:
    DiscardFloatSelectionCommand(MapScene *mapScene,
                                 QList<TileItem*> *floatSelection,
                                 QUndoCommand *parent = nullptr);

public:
    void undo() override;
    void redo() override;

private:
    MapScene               *m_mapScene{nullptr};
    QList<TileItem*>       *m_floatSelectionFromView{nullptr};
    QList<TileItem*>        m_copiedSelection{};
};

#endif // DISCARDFLOATSELECTIONCOMMAND_H
