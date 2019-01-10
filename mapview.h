#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <map>

#include <QDebug>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QTimeLine>

class MapView : public QGraphicsView
{
    Q_OBJECT

public:
    MapView(QWidget *parent = nullptr);

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void createKeys();

private:
    double m_zoomFactor{1.0};
    std::map<std::string, bool> m_keysState{};
};

#endif // MAPVIEW_H
