#ifndef SCREENMANAGERWIDGET_H
#define SCREENMANAGERWIDGET_H

#include <QtWidgets/QWidget>

class Screen;
class ScreenManager;
class ScreenManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenManagerWidget(ScreenManager *manager, QWidget *parent = nullptr);
    Screen* selectedScreen() const;
protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
signals:
    void selectedScreenChanged();
private:
    QPoint scaledPoint(QPointF &sm);
private:
    ScreenManager *m_manager;
    Screen *m_selected = nullptr;
    float m_scale = 0;
};

#endif // SCREENMANAGERWIDGET_H
