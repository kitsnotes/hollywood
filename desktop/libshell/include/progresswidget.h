#ifndef LSOPPROGRESSWIDGET_H
#define LSOPPROGRESSWIDGET_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class LSOpProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LSOpProgressWidget(QWidget *parent = nullptr);

signals:

private:
    QSpacerItem *vs_icon_top;
    QLabel *m_icon;
    QSpacerItem *vs_icon_btm;
    QLabel *m_task;
    QProgressBar *m_progress;
    QToolButton *m_cancelbtn;
    QLabel *m_details;
};

#endif // LSOPPROGRESSWIDGET_H
