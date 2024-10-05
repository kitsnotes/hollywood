// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "getinfodialog.h"
#include "getinfodialog_p.h"
#include "getinfowidgets_p.h"

#include <QApplication>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QGroupBox>
#include <QDateTime>
#include <QMimeType>

#include "mimeapps.h"
#include "desktopentry.h"
#include "hwfileiconprovider.h"

#include <pwd.h>
#include <grp.h>
#include <unistd.h>

LSGetInfoDialogPrivate::LSGetInfoDialogPrivate(const QUrl &target, LSGetInfoDialog *parent)
    : d(parent)
    , m_target(target.path())
    , m_mime(new LSMimeApplications)
    , m_icons(new HWFileIconProvider)
    , m_info(new LSGeneralInfoWidget(this))
    , m_perms(new LSPermissionsAclWidget(this))
{
    //m_mime->cacheAllDesktops();
}

bool LSGetInfoDialogPrivate::isDesktop() const
{
    if(m_target.isEmpty())
        return false;

    QFile f(m_target);
    QMimeDatabase m;
    if(m.mimeTypeForFile(m_target).name() == "application/x-desktop")
        return true;

    return false;
}

QString LSGetInfoDialogPrivate::targetFile() const
{
    return m_target;
}

QFrame *LSGetInfoDialogPrivate::makeLine(QWidget *parent)
{
    auto line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    return line;
}

LSGetInfoDialog::LSGetInfoDialog(const QUrl &file, QWidget *parent)
    : HWPreferenceDialog(parent)
    , p(new LSGetInfoDialogPrivate(file, this))
{
    struct stat sb;
    if(stat(p->m_target.toUtf8(), &sb) == -1)
    {
        // TODO: handle failure;
    }
    if(!p->m_init)
    {
        p->m_inode = sb.st_ino;
        p->m_init = true;
    }

    QFile fi(file.path());
    auto perms = fi.permissions();
    bool canWrite = false;

    if(perms & QFileDevice::WriteUser &&
        sb.st_uid == getuid())
        canWrite = true;

    if(perms & QFileDevice::WriteGroup &&
        sb.st_gid == getgid())
        canWrite = true;

    if(perms & QFileDevice::WriteOther)
        canWrite = true;

    // TODO: check if we have ACL write...

    setFocusPolicy(Qt::ClickFocus);
    setMaximumWidth(425);
    setMinimumWidth(425);
    setWindowTitle(QApplication::tr("Information - %1").arg(file.fileName()));
    // do a quick inspection for a .desktop file
    // we always show this information page *first*
    if(p->isDesktop())
    {
        p->m_desktop = new LSDesktopEntryInfoWidget(p);
        addPage(p->m_desktop, QIcon::fromTheme("kt-info-widget"), tr("Application"));
        p->m_info->disableOpenWith();
        p->m_desktop->reloadInfo();
        if(!canWrite)
            p->m_desktop->setReadOnly();
    }

    addPage(p->m_info, QIcon::fromTheme("kt-info-widget"), tr("Information"));
    addPage(p->m_perms, QIcon::fromTheme("lock"), tr("Permissions"));
    setCurrentIndex(0);

    p->m_info->reloadInfo(sb);
    p->m_perms->reloadInfo(sb);

    if(!canWrite)
    {
        p->m_info->setReadOnly();
        p->m_perms->setReadOnly();
    }
}

void LSGetInfoDialog::open()
{
    HWPreferenceDialog::open();
    p->m_info->m_filename->clearFocus();
    p->m_info->m_openwith->clearFocus();
}
