#include "dialoghelpers.h"
#include <messagebox.h>
#include <QAbstractButton>
#include <QPushButton>

HWMessageDialogHelper::HWMessageDialogHelper()
{
    m_dlg = new HWMessageBox(QMessageBox::NoIcon, QString(), QString());
    connect(m_dlg, &HWMessageBox::accepted, [this]() {{
            accept();
    }});
    connect(m_dlg, &HWMessageBox::rejected, [this]() {{
            reject();
    }});
    connect(m_dlg, &HWMessageBox::buttonClicked, this, &HWMessageDialogHelper::hwDialogButtonClicked);

}

HWMessageDialogHelper::~HWMessageDialogHelper()
{
    m_dlg->deleteLater();
}

void HWMessageDialogHelper::exec()
{
    m_dlg->setIcon((QMessageBox::Icon)options()->icon());
    m_dlg->setText(options()->text());
    m_dlg->setInformativeText(options()->informativeText());
    m_dlg->setWindowTitle(options()->windowTitle());

    uint flags = (uint)options()->standardButtons();
    m_dlg->setStandardButtons(QFlags<QMessageBox::StandardButton>(flags));
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
    m_dlg->setWindowFlags(windowFlags);
    m_dlg->setWindowModality(windowModality);
    //m_dlg->setParent(parent);
    m_dlg->setIcon((QMessageBox::Icon)options()->icon());
    m_dlg->setText(options()->text());
    m_dlg->setInformativeText(options()->informativeText());
    uint flags = (uint)options()->standardButtons();
    m_dlg->setStandardButtons(QFlags<QMessageBox::StandardButton>(flags));
    m_dlg->setDetailedText(options()->detailedText());
    m_dlg->show();
    return true;
}

void HWMessageDialogHelper::hide()
{
    m_dlg->hide();
}

void HWMessageDialogHelper::hwDialogButtonClicked(QAbstractButton *btn)
{
    int btnid = (int)m_dlg->standardButton(btn);
    int btnroleid = (int)m_dlg->buttonRole(btn);
    QPlatformDialogHelper::StandardButton button = (QPlatformDialogHelper::StandardButton)btnid;
    QPlatformDialogHelper::ButtonRole role = (QPlatformDialogHelper::ButtonRole)btnroleid;
    emit clicked(button, role);
}
