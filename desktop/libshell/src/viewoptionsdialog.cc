#include "viewoptionsdialog.h"
#include "iconviewoptions.h"

ViewOptionsDialog::ViewOptionsDialog(QWidget *parent)
    :QDialog(parent),
     m_icons(new IconViewOptions(this))
{
    setWindowTitle(tr("View Options"));
    setWindowFlag(Qt::Tool, true);
    setMaximumWidth(320);
    setMaximumHeight(550);

    QSettings settings("originull", "persistence");
    /* now that the window is configured lets
     * restore our previous settings */
    if(settings.contains("Cache/LastViewOptionsPos"))
        restoreGeometry(settings.value("Cache/LastViewOptionsPos").toByteArray());
    else // fallback defaults
        resize(327, 433);
}

void ViewOptionsDialog::closeEvent(QCloseEvent *event)
{
    QSettings settings("originull", "persistence");
    // preserve our view states
    settings.setValue("Cache/LastViewOptionsPos", saveGeometry());
    QDialog::closeEvent(event);
}

void ViewOptionsDialog::setCurrentView(ArionShell::ViewMode view)
{
    m_viewMode = view;

    switch(m_viewMode)
    {
    case ArionShell::VIEW_LIST:
        m_icons->setVisible(false);
        break;
    case ArionShell::VIEW_COLUMN:
        m_icons->setVisible(false);
        break;
    case ArionShell::VIEW_ICONS:
    default:
        m_icons->setVisible(true);
    }
}

void ViewOptionsDialog::show()
{
    refreshViewOptions();
    QDialog::show();
}

void ViewOptionsDialog::attachIconView(QListView *view)
{
    m_icons->attachView(view);
}

void ViewOptionsDialog::refreshViewOptions()
{
    switch(m_viewMode)
    {
    case ArionShell::VIEW_LIST:
        break;
    case ArionShell::VIEW_COLUMN:
        break;
    case ArionShell::VIEW_ICONS:
    default:
        m_icons->refreshViewOptions();
    }
}
