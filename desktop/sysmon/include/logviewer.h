#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class LogViewer : public QWidget
{
    Q_OBJECT
public:
    explicit LogViewer(QWidget *parent = nullptr);

signals:
private:
    QVBoxLayout *vl_main;
    QSplitter *m_splitter;
    QTreeView *m_sidebar;
    QTextBrowser *m_log;
};

#endif // LOGVIEWER_H
