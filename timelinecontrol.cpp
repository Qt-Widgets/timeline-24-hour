#include "timelinecontrol.h"
#include <random>
#include <set>

using namespace TestProject;

void TimelineControl::generateBooksmarks(const int amount)
{
    m_bookmarks.clear();

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, SecondsPerDay); // (0 - 24) hrs in seconds

    std::uniform_int_distribution<> disDuration(SecondsPerMinute, SecondsPerHour * 3); // (0 - 3) hrs in seconds
    std::multiset<Time> timestamps; // multiset for possible duplicate start timestamps
    for (int i = 0; i < amount; ++i)
    {
        timestamps.insert(dis(rd));
    }

    int i = 1;
    for (auto& time : timestamps)
    {
        Bookmark bookmark;
        bookmark.timestamp = time;
        bookmark.name = "Bookmark " + std::to_string(i++);
        bookmark.duration = disDuration(rd);
        m_bookmarks.insert({ bookmark.timestamp, bookmark });
    }
}

void BookmarksGroup::reset()
{
    m_bookmarksCount = 0;
    m_tooltip = "";
    m_resultDuration = 0;
    m_startTime = 0;
}

void BookmarksGroup::addBookmark(const Bookmark& bookmark)
{
    if (!m_bookmarksCount)
    {
        m_startTime = bookmark.timestamp;
        m_resultDuration = bookmark.duration;
        m_tooltip += bookmark.name;
    }
    else
    {
        if (m_bookmarksCount < TimelineControl::MaxTooltipBookmarks)
        {
            auto addition = '\n' + bookmark.name;
            m_tooltip += addition;
        }

        if ((lastBookmark.timestamp + lastBookmark.duration) < (bookmark.timestamp + bookmark.duration))
        {
            int dif = (bookmark.timestamp + bookmark.duration) - (lastBookmark.timestamp + lastBookmark.duration);
            m_resultDuration += dif;
        }
    }
    m_bookmarksCount++;
    lastBookmark = bookmark;
}
