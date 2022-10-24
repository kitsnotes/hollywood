#include "smanwidget.h"
#include "screenmanager.h"
#include "screen.h"

#include <QPainter>
#include <QString>
#include <QMouseEvent>


float min(float a, float b)
{
    return (a > b) ? b : a;
}

ScreenManagerWidget::ScreenManagerWidget(ScreenManager *manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
{
    setMinimumHeight(200);
    setMinimumWidth(200);
}

void ScreenManagerWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setPen(QColor(Qt::black));
    painter.setBrush(QBrush(QColor(Qt::lightGray)));

    QFont font("Open Sans", 10);

    auto master = m_manager->masterLayoutSize();
    int margin = 0;
    float mina = (float)(width()-margin)/(float)master.width();
    float minb = (float)(height()-margin)/(float)master.height();
    m_scale = min(mina,minb);

    auto rect = QRect(margin/2, margin/2, master.width()*m_scale, master.height()*m_scale);
    painter.fillRect(rect, QColor(Qt::gray));
    painter.drawRect(rect);

    for(auto s : m_manager->screens())
    {
        if(m_selected && s->hash() == m_selected->hash())
        {
            painter.setPen(QColor(Qt::darkRed));
            painter.setBrush(QBrush(QColor(Qt::red)));
        }
        else
        {
            painter.setPen(QColor(Qt::darkGreen));
            painter.setBrush(QBrush(QColor(Qt::green)));
        }

        auto sr = s->rect();
        auto sp = QPointF((float)sr.x()*m_scale,
                          (float)sr.y()*m_scale);
        auto sz = QSizeF((float)sr.width()*m_scale,
                         (float)sr.height()*m_scale);
        auto scale = QRectF(sp, sz);
        painter.fillRect(scale, QColor(Qt::gray));
        painter.drawRect(scale);

        auto indent = QPointF(sp.x()+5, sp.y()+15);
        painter.setPen(QColor(Qt::black));
        painter.drawText(indent,
                         QString("%1")
                         .arg(s->hash()));
        indent.setY((indent.y()+15));
        painter.drawText(indent,
               QString("PT %1,%2")
                         .arg(s->startPoint().x())
                         .arg(s->startPoint().y()));
        indent.setY((indent.y()+15));
        painter.drawText(indent,
               QString("SZ %1x%2")
                         .arg(s->resolution().width())
                         .arg(s->resolution().height()));
        indent.setY((indent.y()+15));
        painter.drawText(indent,
               QString("Row %1")
                         .arg(s->row()));

    }
}

void ScreenManagerWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        e->accept();
        auto point = e->points().first().position();
        auto displayPoint = scaledPoint(point);
        auto display = m_manager->screenAt(displayPoint);
        if(!display)
        {
            m_selected = nullptr;
            qDebug() << "clicked on a dead zone" << displayPoint;
        }
        else
        {
            m_selected = display;
            qDebug() << "clicked on display" << display->hash() << "at point" << displayPoint
                     << "usable edges: top" << display->edgeUsable(Qt::TopEdge)
                     << "left" << display->edgeUsable(Qt::LeftEdge)
                     << "right" << display->edgeUsable(Qt::RightEdge)
                     << "bottom" << display->edgeUsable(Qt::BottomEdge);

        }
        emit selectedScreenChanged();
        update();
    }
    QWidget::mousePressEvent(e);
}

Screen *ScreenManagerWidget::selectedScreen() const
{
    if(m_selected)
        return const_cast<Screen*>(m_selected);

    return nullptr;
}

QPoint ScreenManagerWidget::scaledPoint(QPointF &sm)
{
    return QPoint(sm.x()/m_scale, sm.y()/m_scale);
}
