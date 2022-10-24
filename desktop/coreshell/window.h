#ifndef CORESHELL_H
#define CORESHELL_H

#include <QWidget>
#include <QListView>

class CSWindow : public QWidget
{
    Q_OBJECT

public:
    CSWindow(QScreen *screen);
    ~CSWindow();
    QScreen* screen();
private slots:
    void geometryChanged(const QRect &geom);
    void activated(const QModelIndex &idx);
    void contextMenu(const QPoint &pos);
private:
    QScreen *m_screen = nullptr;
    QListView *m_view = nullptr;
};
#endif // CORESHELL_H
