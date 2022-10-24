#ifndef PROCESSINFODIALOG_H
#define PROCESSINFODIALOG_H

#include <QDialog>
#include <QWidget>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QTableView>

class ProcessInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ProcessInfoDialog(pid_t process, const QString &icon = QString(), QWidget *parent = nullptr);
private slots:
    void terminateProcess();
private:
    void populateDetails();
private:
    pid_t m_process;
    QString m_procname;
    QString m_icon;
    QVBoxLayout *mainLayout;
    QLabel *lb_icon;
    QLabel *lb_procname;
    QLineEdit *m_cmdline;
    QLineEdit *m_cwd;
    QTabWidget *m_tabs;
    QTableView *m_env;
    QTableView *m_files;
    QTableView *m_memory;
    QToolButton *m_help;
    QToolButton *m_signal;
    QPushButton *m_kill;
    QPushButton *m_close;
};

#endif // PROCESSINFODIALOG_H
