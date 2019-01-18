#ifndef FILLALLCOMMAND_H
#define FILLALLCOMMAND_H

#include "mapscene.h"

#include <QUndoCommand>

class FillAllCommand : public QUndoCommand
{
public:
    FillAllCommand(MapScene *mapScene,
                   const QString& textureName,
                   QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    MapScene *m_mapScene{nullptr};
    QString m_textureName{};
    std::vector<QString> m_oldTilesTexturesNames{};
};

#endif // FILLALLCOMMAND_H
