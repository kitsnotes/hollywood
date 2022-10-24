#ifndef NICEDIALOG_H
#define NICEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>


class NiceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NiceDialog(pid_t process, const QString &icon = QString(), QWidget *parent = nullptr);
private:
    pid_t m_process;
    QString m_icon;
    QVBoxLayout *verticalLayout;
    QLabel *lb_icon;
    QLabel *lb_procname;
    QSpinBox *sb_nice;
    QSlider *sld_nice;
    QToolButton *m_help;
    QPushButton *m_cancel;
    QPushButton *m_ok;
};

#endif // NICEDIALOG_H
