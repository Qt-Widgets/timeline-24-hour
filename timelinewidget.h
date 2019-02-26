#ifndef TIMELINE_WIDGET_H
#define TIMELINE_WIDGET_H

#include "timelinecontrol.h"

#include <set>

#include <QWidget>


namespace TestProject
{

/*!
\brief Result graph element
*/
struct GraphElement
{
    QRectF rect;
    std::string name;

    bool operator <(const GraphElement& rhv) const
    {
        return this->rect.x() < rhv.rect.x();
    }
};

class TimelineWidget : public QWidget
{
    Q_OBJECT
public:
    TimelineWidget(TimelineControl& control, QWidget* parent = nullptr);

    void drawBookmark(QPainter& painter, const QRect &rect, const QString& label);
    void drawGroup(QPainter& painter, const QRect &rect, const QString& label);
    bool drawTimeline(QPainter& painter, int leftTime, int rightTime);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event);

private:
    void zoomIn(const QPoint& mousePos);
    void zoomOut(const QPoint& mousePos);


private:
    TimelineControl& m_control;
    std::multiset<GraphElement> m_elements;
};

}

#endif