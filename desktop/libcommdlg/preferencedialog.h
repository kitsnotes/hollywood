#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "libcommdlg_global.h"

#include <QDialog>
#include <QWidget>
#include <QList>

class QToolBar;
class QAction;
class LIBCOMMDLG_EXPORT ADPreferenceDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool animated READ animated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    ADPreferenceDialog(QWidget *parent = nullptr);
    ~ADPreferenceDialog();
    void addPage(QWidget *page, const QIcon &icon, const QString &title);
    bool animated();
    int currentIndex();
public slots:
    void setAnimated(bool animated);
    void setCurrentIndex(int index);
    void show();
signals:
    void animatedChanged();
    void currentIndexChanged(int index);
private slots:
    void tabActionTriggered();
private:
    bool m_animate = true;
    int m_index = 0;
    QToolBar *m_tool = nullptr;
    QWidget *m_placeholder = nullptr;
    QWidget *m_current = nullptr;
    QList<QAction*> m_tabs;
    QList<QWidget*> m_tabcontent;
    QAction *m_leftSpacerAct = nullptr;
    QAction *m_rightSpacerAct = nullptr;
};

#endif // PREFERENCEDIALOG_H
