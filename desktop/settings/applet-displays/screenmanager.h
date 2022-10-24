#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <QObject>
#include <QSize>
#include <QRect>
#include <QMap>
#include <QScreen>

class Screen;
class ScreenManager : public QObject
{
    Q_OBJECT
public:
    explicit ScreenManager(QObject *parent = nullptr);
    Screen* createFirstScreen(QScreen *screen);
    Screen* createExtensionScreen(QScreen *screen, Screen *buddy, Qt::Edge anchor, int offset, bool mirror = false);

    QSize masterLayoutSize();
    QList<Screen*> screens() { return m_screens; }
    Screen* screenAt(QPoint &p) const;
    Qt::Edge oppositeEdge(Qt::Edge edge);
signals:
private:
private:
    void recursiveAdjustWidth(Screen *s, int width);
    void recursiveAdjustHeight(Screen *s, int height);
    void prependRow(int height);
    void appendRow(int height);
    void adjustRowHeight(int row, int height);
    QSize m_virtual_size;
    QList<Screen*> m_screens;
    int m_rows = 0;
    QList<int> m_rowSizes;
    Screen* m_primary = nullptr;
};

#endif // SCREENMANAGER_H
