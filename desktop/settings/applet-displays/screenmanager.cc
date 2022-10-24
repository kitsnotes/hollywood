#include "screenmanager.h"
#include "screen.h"

#include <QDebug>
ScreenManager::ScreenManager(QObject *parent)
    : QObject{parent}
{

}

Screen *ScreenManager::createFirstScreen(QScreen *qs)
{
    auto screen = new Screen(this);
    screen->m_mode = Screen::Primary;
    screen->m_resolution = qs->size();
    screen->m_hash = QString("%1-%2-%3").arg(qs->manufacturer(), qs->model(), qs->serialNumber());
    prependRow(qs->size().height());
    m_screens.append(screen);
    m_primary = screen;
    m_virtual_size = qs->size();
    return screen;
}

Screen *ScreenManager::createExtensionScreen(QScreen *qs, Screen *buddy, Qt::Edge anchor, int offset, bool mirror)
{
    // anchor is reflective **as of the buddy**
    if(!m_primary)
    {
        qWarning() << "ScreenManager::createExtensionScreen attempted without a primary screen.";
        return nullptr;
    }

    if(!buddy)
    {
        qWarning() << "ScreenManager::createExtensionScreen: invalid buddy";
        return nullptr;
    }

    auto screen = new Screen(this);
    if(mirror)
        screen->m_mode = Screen::Mirror;
    else
        screen->m_mode = Screen::Extension;
    screen->m_resolution = qs->size();
    screen->m_hash = QString("%1-%2-%3").arg(qs->manufacturer(), qs->model(), qs->serialNumber());
    auto edge = oppositeEdge(anchor);
    screen->m_buddy = buddy;
    // we store the anchor as to this screen (opposite)
    screen->m_anchor = edge;
    screen->m_offset = offset;

    // expand our virtual size if needed
    if(anchor == Qt::RightEdge ||
       anchor == Qt::LeftEdge)
        m_virtual_size.setWidth(m_virtual_size.width()+qs->size().width());
     else
        m_virtual_size.setHeight(m_virtual_size.height()+qs->size().height());

    qDebug() << "adjusted virtual height to" << m_virtual_size;
    if(buddy->m_neighbors[anchor] != nullptr)
        qWarning() << "ScreenManager::createExtensionScreen: setting neighbor where there already is one";

    buddy->m_neighbors[anchor] = screen;
    screen->m_neighbors[edge] = buddy;

    if(anchor == Qt::LeftEdge)
    {
        // adjust our current displays in this row position over
        recursiveAdjustWidth(buddy, qs->size().width());
        // TODO: if row not 1 then make sure we have positioning ability
        screen->m_start.setY(buddy->m_start.y());
    }
    if(anchor == Qt::RightEdge)
    {
        screen->m_start.setY(buddy->m_start.y());
        screen->m_start.setX(buddy->m_start.x()+buddy->m_resolution.width()+1);
    }

    if(anchor == Qt::TopEdge)
    {
        // add a new display row if required
        if(buddy->m_row == 0)
            prependRow(screen->m_resolution.height());
        else // otherwise make sure the row is tall enough
            adjustRowHeight(buddy->m_row-1, screen->m_resolution.height());
        screen->m_start.setX(buddy->m_start.x());
    }
    if(anchor == Qt::BottomEdge)
    {
        screen->m_start.setX(buddy->m_start.x());
        screen->m_row = buddy->row()+1;
        appendRow(screen->m_resolution.height());
        screen->m_start.setY(buddy->m_start.y()+buddy->m_resolution.height()+1);
    }

    m_screens.append(screen);
    return screen;
}

QSize ScreenManager::masterLayoutSize()
{
    return m_virtual_size;
}

Screen *ScreenManager::screenAt(QPoint &p) const
{
    for(auto s : m_screens)
    {
        if(s->rect().contains(p))
            return const_cast<Screen*>(s);
    }

    return nullptr;
}

Qt::Edge ScreenManager::oppositeEdge(Qt::Edge edge)
{
    switch(edge)
    {
    case Qt::RightEdge:
        return Qt::LeftEdge;
    case Qt::LeftEdge:
        return Qt::RightEdge;
    case Qt::TopEdge:
        return Qt::BottomEdge;
    case Qt::BottomEdge:
        return Qt::TopEdge;
    }
}

void ScreenManager::recursiveAdjustWidth(Screen *s, int width)
{
    if(s == nullptr)
        return;

    s->m_start.setX(s->m_start.x()+width);
    recursiveAdjustWidth(s->m_neighbors[Qt::RightEdge], width);
}

void ScreenManager::recursiveAdjustHeight(Screen *s, int height)
{
    if(s == nullptr)
        return;

    s->m_start.setY(s->m_start.y()+height);
    recursiveAdjustHeight(s->m_neighbors[Qt::BottomEdge], height);
}

void ScreenManager::prependRow(int height)
{
    m_rows++;
    m_rowSizes.prepend(height);
    for(auto s : m_screens)
    {
        s->m_row++;
        s->m_start.setY(s->m_start.y()+height+1);
    }
}

void ScreenManager::appendRow(int height)
{
    m_rows++;
    m_rowSizes.append(height);
}

void ScreenManager::adjustRowHeight(int row, int height)
{
    if(m_rowSizes[row] < height)
        m_rowSizes[row] = height;
}

