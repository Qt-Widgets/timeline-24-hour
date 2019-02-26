#include "timelinewidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QToolTip>

#include <algorithm>

using namespace TestProject;

static constexpr int MinDistance = 100;

static constexpr int BookmarkHeight = 14;
static constexpr int TimelineOffsetY = 10;
static constexpr int HoursMarkLenght = 10;
static constexpr int HoursMarkOffsetY = 12;

static constexpr int BookmarkRectOffsetY = TimelineOffsetY + HoursMarkLenght + BookmarkHeight;

static constexpr int TextmarkOffsetX = 2;
static constexpr int TextmarkOffsetY = 11;

TimelineWidget::TimelineWidget(TimelineControl& control, QWidget* parent)
    : QWidget(parent), m_control(control)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    this->setMouseTracking(true);
}

void TimelineWidget::mouseMoveEvent(QMouseEvent* event)
{
    auto position = event->localPos();
    // need to find the position in sorted array
    auto element = std::lower_bound(std::cbegin(m_elements), std::cend(m_elements), position, [&](const auto &rhv, const auto &value)
    {
        return (rhv.rect.x() + rhv.rect.width())  < value.x();
    });

    if (element != m_elements.end())
    {
        if (position.x() >= element->rect.x()
            && position.x() < element->rect.x() + element->rect.width()
            && position.y() >= element->rect.y()
            && position.y() < element->rect.y() + element->rect.height())
        {
            // check for nested bookmark
            auto nextElement = std::next(element);
            if (nextElement != m_elements.end())
            {
                if (position.x() >= nextElement->rect.x()
                    && position.x() < nextElement->rect.x() + nextElement->rect.width())
                {
                    QToolTip::showText(event->globalPos(), QString::fromStdString(nextElement->name));
                    return;
                }
            }
            QToolTip::showText(event->globalPos(), QString::fromStdString(element->name));
        }
    }
}

void TimelineWidget::wheelEvent(QWheelEvent *event)
{
    auto mousePosition = event->pos();
    if (event->delta() > 0) // forward
    {
        this->zoomIn(mousePosition);
    }
    else
    {
        this->zoomOut(mousePosition);
    }
    this->update();
    event->accept();
}

void TimelineWidget::zoomIn(const QPoint& /*mousePos*/)
{

}

void TimelineWidget::zoomOut(const QPoint& mousePos)
{

}

void TimelineWidget::paintEvent(QPaintEvent* /*event*/ )
{
    m_elements.clear();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    if (!drawTimeline(painter, m_control.leftBorder(), m_control.rightBorder()))
        return;

    // draw bookmarks
    if (m_control.bookmarkAmount())
    {
        auto currentRect = this->rect();
        decltype(auto) bookmarks = m_control.bookmarks();
        auto oneSecDim = currentRect.width() / static_cast<double>(TimelineControl::SecondsPerDay);

        BookmarksGroup currentGroup;
        for (auto currentBookmark = bookmarks.cbegin(); currentBookmark != bookmarks.cend(); ++currentBookmark)
        {
            auto nextBookmark = std::next(currentBookmark);
            auto currentPos = currentBookmark->second.timestamp * oneSecDim;
            bool draw = true;
            if (nextBookmark != bookmarks.end())
            {
                auto nextPos = nextBookmark->second.timestamp * oneSecDim;
                if ((nextPos - currentPos) <= MinDistance)
                {
                    draw = false;
                    if (!currentGroup.bookmarksCount())
                    {
                        currentGroup.addBookmark(currentBookmark->second);
                        currentGroup.addBookmark(nextBookmark->second);
                    }
                    else
                    {
                        currentGroup.addBookmark(nextBookmark->second);
                    }
                }
            }

            if (draw)
            {
                auto groupBookmarksCount = currentGroup.bookmarksCount();
                if (groupBookmarksCount)
                {
                    if (groupBookmarksCount > TimelineControl::MaxTooltipBookmarks)
                    {
                        auto addition = "\n +" + std::to_string(groupBookmarksCount - TimelineControl::MaxTooltipBookmarks) + " other bookmarks";
                        currentGroup.setTooltip(currentGroup.tooltip() + addition);
                    }
                    int x = currentGroup.startTime() * oneSecDim;
                    int width = currentGroup.resultDuration() * oneSecDim;
                    QRect rect = { x, currentRect.y() + BookmarkRectOffsetY, width, BookmarkHeight };

                    drawGroup(painter, rect, QString::number(groupBookmarksCount));
                    m_elements.insert({ rect, currentGroup.tooltip() });
                    currentGroup.reset();
                }
                else
                {
                    int width = std::ceil(currentBookmark->second.duration * oneSecDim);
                    auto rect = QRect{ static_cast<int>(currentPos), currentRect.y() + BookmarkRectOffsetY, width, BookmarkHeight };
                    drawBookmark(painter, rect, QString::fromStdString(currentBookmark->second.name));
                    m_elements.insert({ rect, currentBookmark->second.name });
                }
            }
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

bool TimelineWidget::drawTimeline(QPainter& painter, int leftTime, int rightTime)
{
    if (leftTime >= rightTime)
        return false;

    auto currentRect = this->rect();
    if (!currentRect.isValid())
        return false;

    // draw scale
    QPoint left = currentRect.topLeft();
    left.setY(left.y() + TimelineOffsetY);
    QPoint right = currentRect.topRight();
    right.setY(right.y() + TimelineOffsetY);
    painter.drawLine(left, right);

    auto secondsInWidth = rightTime - leftTime;
    auto oneSecDimension = currentRect.width() / (double)secondsInWidth; // pixels in one second
    for (auto currentTime = leftTime; currentTime <= rightTime;)
    {
        auto leftToHour = currentTime % TimelineControl::SecondsPerHour;
        auto hoursPassed = currentTime / TimelineControl::SecondsPerHour;
        auto xOffset = (currentTime)* oneSecDimension;
        QPoint startMarkPoint = { left.x() + static_cast<int>(xOffset), left.y() };
        QPoint endMarkPoint = { startMarkPoint.x(), startMarkPoint.y() + HoursMarkLenght };
        painter.drawLine(startMarkPoint, endMarkPoint);

        if (leftToHour != 0)
        {
            hoursPassed++;
        }
        auto textMark = QString::number(hoursPassed) + "h";
        if (hoursPassed == TimelineControl::HoursPerDay)
        {
            textMark = "0h";
        }
        QFontMetrics fm = painter.fontMetrics();
        double width = fm.width(textMark);
        QPoint textPoint = { endMarkPoint.x() - static_cast<int>(width / 2), endMarkPoint.y() + HoursMarkOffsetY };
        painter.drawText(textPoint, textMark);
        currentTime += TimelineControl::SecondsPerHour;
    }
    return true;
}

void TimelineWidget::drawBookmark(QPainter& painter, const QRect &rect, const QString& label)
{
    painter.save();
    painter.setBrush(QColor(92, 144, 226, 192));
    painter.setPen(Qt::PenStyle::NoPen);

    if (!rect.width())
    {
        auto newRect = rect;
        newRect.setWidth(1);
        painter.drawRect(newRect);
    }
    else
    {
        painter.drawRect(rect);
    }

    QPoint painterOffset = { rect.x() + TextmarkOffsetX, (rect.y() + 11) };
    auto text = label;
    QFontMetrics fm = painter.fontMetrics();
    int labelWidth = fm.width(label);
    QString dot = ".";
    if (rect.width() < labelWidth)
    {
        auto charWidth = fm.charWidth(text, 0);
        auto charFit = rect.width() / charWidth;
        if (charFit == 0)
        {
            text = dot;
        }
        else
        {
            if (text.length() != charFit)
            {
                text = text.mid(0, charFit - 1);
                text.append(dot);
            }
        }
    }
    painter.setPen(Qt::white);
    painter.drawText(painterOffset, text);
    painter.restore();
}

void TimelineWidget::drawGroup(QPainter& painter, const QRect &rect, const QString& label)
{
    painter.save();
    painter.setBrush(QColor(105, 204, 118, 192));
    painter.setPen(Qt::PenStyle::NoPen);
    painter.drawRect(rect);
    
    QPoint painterOffset = { rect.x() + TextmarkOffsetX, (rect.y() + TextmarkOffsetY) };
    painter.setPen(Qt::white);
    painter.drawText(painterOffset, label);
    painter.restore();
}