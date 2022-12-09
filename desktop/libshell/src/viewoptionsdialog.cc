#include "viewoptionsdialog.h"
#include "viewoptionsdialog_p.h"
#include "iconviewoptions.h"

LSViewOptionsDialogPrivate::LSViewOptionsDialogPrivate(LSViewOptionsDialog *parent)
    : d(parent)
    , m_layout(new QVBoxLayout(parent))
    , m_icons(new LSIconViewOptions(parent))
{

}

LSViewOptionsDialog::LSViewOptionsDialog(QWidget *parent)
    : QDialog(parent)
    , p(new LSViewOptionsDialogPrivate(this))
{
    setWindowTitle(tr("View Options"));
    setWindowFlag(Qt::Tool, true);
    setMaximumWidth(320);
    setMaximumHeight(550);
    setLayout(p->m_layout);
    p->m_layout->setContentsMargins(0,0,0,0);

    QSettings settings("originull", "persistence");
    /* now that the window is configured lets
     * restore our previous settings */
    if(settings.contains("Cache/LastViewOptionsPos"))
        restoreGeometry(settings.value("Cache/LastViewOptionsPos").toByteArray());
    else // fallback defaults
        resize(327, 433);
}

void LSViewOptionsDialog::closeEvent(QCloseEvent *event)
{
    QSettings settings("originull", "persistence");
    // preserve our view states
    settings.setValue("Cache/LastViewOptionsPos", saveGeometry());
    QDialog::closeEvent(event);
}

void LSViewOptionsDialog::setCurrentView(HWShell::ViewMode view)
{
    if(p->m_current)
    {
        p->m_layout->removeWidget(p->m_current);
        p->m_current = nullptr;
    }
    p->m_viewMode = view;

    switch(p->m_viewMode)
    {
    case HWShell::VIEW_LIST:
        p->m_icons->setVisible(false);
        break;
    case HWShell::VIEW_COLUMN:
        p->m_icons->setVisible(false);
        break;
    case HWShell::VIEW_ICONS:
    default:
        p->m_icons->setVisible(true);
        p->m_current = p->m_icons;
        p->m_layout->addWidget(p->m_icons);
    }
}

void LSViewOptionsDialog::show()
{
    refreshViewOptions();
    QDialog::show();
}

void LSViewOptionsDialog::attachIconView(QListView *view)
{
    p->m_icons->attachView(view);
}

void LSViewOptionsDialog::refreshViewOptions()
{
    switch(p->m_viewMode)
    {
    case HWShell::VIEW_LIST:
        break;
    case HWShell::VIEW_COLUMN:
        break;
    case HWShell::VIEW_ICONS:
    default:
        p->m_icons->refreshViewOptions();
    }
}

