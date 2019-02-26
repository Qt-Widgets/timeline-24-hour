#ifndef TIMELINE_CONTROL_H
#define TIMELINE_CONTROL_H

#include <string>
#include <random>
#include <set>
#include <map>

#include "QtConcurrent\qtconcurrentrun.h"
#include <QFutureWatcher>
#include <future>
#include <QEventLoop>

namespace TestProject
{

using Time = int;

struct Bookmark
{
    std::string name;
    Time timestamp; // s
    Time duration; // s
};

/*!
\brief Struct for grouping bookmarks
*/
class BookmarksGroup
{
public:
    BookmarksGroup() = default;
    BookmarksGroup(const BookmarksGroup& rh) = delete;
    BookmarksGroup& operator=(const BookmarksGroup& rh) = delete;

    void addBookmark(const Bookmark& bookmark);

    void reset();
    void setTooltip(const std::string& text) { m_tooltip = text; }

    auto resultDuration() const { return m_resultDuration; }
    auto bookmarksCount() const { return m_bookmarksCount; }
    auto startTime() const { return m_startTime; }
    auto tooltip() const { return m_tooltip; }

private:
    Time m_resultDuration = 0;
    Time m_startTime = 0;
    int m_bookmarksCount = 0;
    std::string m_tooltip = "";

    Bookmark lastBookmark;
};


class TimelineControl
{
public:
    static constexpr int SecondsPerDay = 86400;
    static constexpr int SecondsPerHour = 3600;
    static constexpr int SecondsPerMinute = 60;
    static constexpr int HoursPerDay = 24;

    static constexpr int MaxTooltipBookmarks = 15; // max count of bookmarks in group for display in a tooltip

    TimelineControl() = default;
    TimelineControl(const TimelineControl& rh) = delete;
    TimelineControl& operator=(const TimelineControl& rh) = delete;

    void generateBooksmarks(const int amount);

    size_t bookmarkAmount() const { return m_bookmarks.size(); };
    const std::multimap<Time, Bookmark>& bookmarks() const { return m_bookmarks; }

    auto leftBorder() const { return m_leftBorder; }
    auto rightBorder() const { return m_rightBorder; }

    void zoomIn(int x, double scaleRatio);
    void zoomOut(int x, double scaleRatio);

private:
    std::multimap<Time, Bookmark> m_bookmarks;

    int m_leftBorder = 0; // left border of FOV in seconds
    int m_rightBorder = SecondsPerDay; // right border of FOV in seconds
};

}

#endif