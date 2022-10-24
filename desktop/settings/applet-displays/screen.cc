#include "screen.h"
#include "screenmanager.h"
#include <QScreen>

Screen::Screen(ScreenManager *parent)
    : QObject(parent)
    , m_parent(parent)
    , m_start(QPoint(0,0))
{
    m_neighbors[Qt::TopEdge] = nullptr;
    m_neighbors[Qt::BottomEdge] = nullptr;
    m_neighbors[Qt::LeftEdge] = nullptr;
    m_neighbors[Qt::RightEdge] = nullptr;
}

QString Screen::hash() const
{
    return m_hash;
}

Screen::Mode Screen::mode() const
{
    return m_mode;
}

QSize Screen::resolution() const
{
    return m_resolution;
}

QPoint Screen::startPoint() const
{
    return m_start;
}

QRect Screen::rect() const
{
    return QRect(m_start, m_resolution);
}

int Screen::row() const
{
    return m_row;
}

Qt::Edge Screen::buddyAnchor()
{
    return m_anchor;
}

bool Screen::edgeUsable(Qt::Edge edge)
{
    // we prefer having alignments to sides rather than top/bottom
    if(edge == Qt::TopEdge)
    {
        if(m_neighbors[Qt::TopEdge] != nullptr)
            return false;

        auto testpoint = QPoint(m_start);
        testpoint.setY(testpoint.y()-10);
        testpoint.setX(testpoint.x()+2);
        return !m_parent->screenAt(testpoint);
    }
    if(edge == Qt::BottomEdge)
    {
        if(m_neighbors[Qt::BottomEdge] != nullptr)
            return false;
        auto testpoint = QPoint(m_start);
        testpoint.setY(testpoint.y()+m_resolution.height()+10);
        testpoint.setX(testpoint.x()+2);
        return !m_parent->screenAt(testpoint);
    }

    if(edge == Qt::LeftEdge)
        if(m_neighbors[Qt::LeftEdge] != nullptr)
            return false;

    if(edge == Qt::RightEdge)
        if(m_neighbors[Qt::RightEdge] != nullptr)
            return false;

    return true;
}
