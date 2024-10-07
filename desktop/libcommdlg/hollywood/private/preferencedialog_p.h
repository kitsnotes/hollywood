#ifndef PREFERENCEDIALOG_P_H
#define PREFERENCEDIALOG_P_H

#include <QWidget>
#include <QToolBar>
#include <QSpacerItem>

class HWPreferenceDialog;
class HWPreferenceDialogPrivate
{
private:
    friend class HWPreferenceDialog;
    HWPreferenceDialogPrivate(HWPreferenceDialog *parent);
    ~HWPreferenceDialogPrivate() = default;
    HWPreferenceDialog *d;
    bool m_animate = true;
    int m_index = -1;
    QToolBar *m_tool = nullptr;
    QWidget *m_placeholder = nullptr;
    QWidget *m_current = nullptr;
    QList<QAction*> m_tabs;
    QList<QWidget*> m_tabcontent;
    QAction *m_leftSpacerAct = nullptr;
    QAction *m_rightSpacerAct = nullptr;
    QSpacerItem *m_spacer = nullptr;

    bool m_first_animate = false;
};

#endif // PREFERENCEDIALOG_P_H
