#include "dialoghelpers.h"
#include <hollywood/messagebox.h>
#include <QAbstractButton>
#include <QPushButton>

HWMessageDialogHelper::HWMessageDialogHelper()
{
    m_dlg = nullptr;
}

HWMessageDialogHelper::~HWMessageDialogHelper()
{
    if(m_dlg)
        m_dlg->deleteLater();
}

void HWMessageDialogHelper::exec()
{
    uint flags = (uint)options()->standardButtons();
#if QT_VERSION >= 0x060600
    provision((QMessageBox::Icon)options()->standardIcon(),
              options()->windowTitle(),
              options()->text(),
              QFlags<QMessageBox::StandardButton>(flags));
#else
    provision((QMessageBox::Icon)options()->icon(),
              options()->windowTitle(),
              options()->text(),
              QFlags<QMessageBox::StandardButton>(flags));
#endif
    m_dlg->setInformativeText(options()->informativeText());
    m_dlg->setDetailedText(options()->detailedText());
    if(options()->customButtons().count() > 0)
    {
        for(auto &button : options()->customButtons())
        {
            QPushButton *custom = new QPushButton(button.label, m_dlg);
            int irole = (int)button.role;
            QMessageBox::ButtonRole role = (QMessageBox::ButtonRole)irole;
            m_dlg->addButton(qobject_cast<QAbstractButton*>(custom), role);
        }
    }
    m_dlg->exec();
}

bool HWMessageDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    Q_UNUSED(parent)
    if(m_dlg)
    {
        m_dlg->setWindowFlags(windowFlags);
        m_dlg->setWindowModality(windowModality);
        m_dlg->show();
    }
    return true;
}

void HWMessageDialogHelper::hide()
{
    m_dlg->hide();
}

void HWMessageDialogHelper::provision(QMessageBox::Icon icon, const QString &title, const QString &text,
                                      QMessageBox::StandardButtons buttons)
{
    if(m_dlg != nullptr)
        m_dlg->deleteLater();

    m_dlg = new HWMessageBox(icon, title, text, buttons);
    connect(m_dlg, &HWMessageBox::accepted, [this]() {{
            accept();
    }});
    connect(m_dlg, &HWMessageBox::rejected, [this]() {{
            reject();
    }});
    connect(m_dlg, &HWMessageBox::buttonClicked, this, &HWMessageDialogHelper::hwDialogButtonClicked);
}

void HWMessageDialogHelper::hwDialogButtonClicked(QAbstractButton *btn)
{
    int btnid = (int)m_dlg->standardButton(btn);
    int btnroleid = (int)m_dlg->buttonRole(btn);
    QPlatformDialogHelper::StandardButton button = (QPlatformDialogHelper::StandardButton)btnid;
    QPlatformDialogHelper::ButtonRole role = (QPlatformDialogHelper::ButtonRole)btnroleid;
    emit clicked(button, role);
}
