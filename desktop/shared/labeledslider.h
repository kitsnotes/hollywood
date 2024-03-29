#ifndef LABELEDSLIDER_H
#define LABELEDSLIDER_H

#include <QSlider>
#include <QObject>
#include <QWidget>

class LabeledSlider : public QSlider
{
    Q_OBJECT
public:
    LabeledSlider(QWidget *parent = nullptr);
    QSize minimumSizeHint() const override;
    void setTickLabels(QMap<int,QString> &labels);
protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;
private:
    bool m_drawNums = true;
    int m_firstWidth = 0;
    int m_offset = 0;
    QMap<int,QString> m_tickLabels;
};

#endif // LABELEDSLIDER_H
