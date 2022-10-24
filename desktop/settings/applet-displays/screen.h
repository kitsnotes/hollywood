#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include <QSize>
#include <QRect>
#include <QMap>

class ScreenManager;
class Screen : public QObject
{
    Q_OBJECT
public:
    enum Mode {
        Primary,
        Extension,
        Mirror
    };
    QString hash() const;
    Mode mode() const;
    QSize resolution() const;
    QPoint startPoint() const;
    QRect rect() const;
    int row() const;
    Screen* buddy() const;
    Qt::Edge buddyAnchor();
    bool edgeUsable(Qt::Edge edge);
protected:
signals:

private:
    friend class ScreenManager;
    explicit Screen(ScreenManager *parent = nullptr);
    QString m_hash;
    Mode m_mode;
    QSize m_resolution;

    // buddy,offset,anchor
    // should always be null if primary
    Screen* m_buddy = nullptr;
    int m_offset = 0;
    Qt::Edge m_anchor;

    QMap<Qt::Edge, Screen*> m_neighbors;
    ScreenManager *m_parent;

    QPoint m_start;
    int m_row = 0;
};

#endif // SCREEN_H
