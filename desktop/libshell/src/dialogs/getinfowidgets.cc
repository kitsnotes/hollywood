// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#include "getinfowidgets_p.h"
#include "getinfodialog_p.h"
#include "getinfodialog.h"

#include "hwfileiconprovider.h"
#include "mimeapps.h"
#include "desktopentry.h"
#include "sectionwidget.h"
#include "commonfunctions.h"
#include "disks.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QMimeDatabase>
#include <QDateTime>
#include <sys/statvfs.h>
#include <pwd.h>
#include <grp.h>

#ifdef USE_APK
#include <ApkQt/QtApk>
#endif

LSGeneralInfoWidget::LSGeneralInfoWidget(LSGetInfoDialogPrivate *parent)
    : QWidget(parent->d)
    , m_pixmap(new QLabel(this))
    , m_filename(new QLineEdit(this))
    , m_fileinfo(new QLabel(this))
    , m_filesize(new QLabel(this))
    , m_mimetype(new QLabel(this))
    , m_labels(new QFormLayout)
    , m_openwithlabel(new QLabel(this))
    , m_openwith(new QComboBox(this))
    , m_location(new QLabel(this))
    , m_disksize(new QLabel(this))
    , m_ctime(new QLabel(this))
    , m_mtime(new QLabel(this))
    , m_atime(new QLabel(this))
    , p(parent)
{
    auto font = m_filesize->font();
    font.setPointSize(font.pointSize()+1);
    m_filesize->setFont(font);
    m_fileinfo->setIndent(15);
    m_filesize->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    m_pixmap->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    // setup the main view
    auto vb_main = new QVBoxLayout(this);

    auto hl_files = new QHBoxLayout;
    auto vl_filename = new QVBoxLayout;
    vl_filename->addWidget(m_filename);
    vl_filename->addWidget(m_fileinfo);
    hl_files->addWidget(m_pixmap);
    hl_files->addLayout(vl_filename);
    hl_files->addWidget(m_filesize);

    auto l_loc = new QLabel(QApplication::tr("Location:"), this);
    auto l_ds = new QLabel(QApplication::tr("Size on disk:"), this);
    auto l_mt = new QLabel(QApplication::tr("MIME type:"), this);
    auto l_ct = new QLabel(QApplication::tr("Created:"), this);
    auto l_mod = new QLabel(QApplication::tr("Modified:"), this);
    auto l_at = new QLabel(QApplication::tr("Accessed:"), this);

    if(p->m_target.startsWith("/dev/"))
    {
        // we are supplied a /dev link ....
        auto dev =
            LSCommonFunctions::instance()->udiskManager()->
            deviceForDevPath(p->m_target);
        if(dev)
            m_mountDevice = dev;
    }
    else
    {
        auto dev =
            LSCommonFunctions::instance()->udiskManager()->
            deviceForMountpath(p->m_target);

        if(dev)
            m_mountDevice = dev;
    }

    if(m_mountDevice)
    {
        l_loc->setText(QApplication::tr("Mount Point:"));
        l_mt->setText(QApplication::tr("UNIX Device:"));
        l_ds->setText(QApplication::tr("Filesystem type:"));
        l_ct->setText(QApplication::tr("Capacity:"));
        l_mod->setText(QApplication::tr("Available:"));
        l_at->setText(QApplication::tr("Used:"));
        if(m_mountDevice->mountpoint().isEmpty())
        {
            l_mod->setVisible(false);
            l_at->setVisible(false);
            m_mtime->setVisible(false);
            m_atime->setVisible(false);
        }
    }

    m_openwithlabel->setText(QApplication::tr("Open with:"));

    m_labels->setLabelAlignment(Qt::AlignRight);
    m_labels->addRow(l_loc, m_location);
    m_labels->addRow(l_mt, m_mimetype);
    m_labels->addRow(l_ds, m_disksize);
    m_labels->addRow(l_ct, m_ctime);
    m_labels->addRow(l_mod, m_mtime);
    m_labels->addRow(l_at, m_atime);
    m_beforeOpenWithLine = p->makeLine(this);
    m_labels->setWidget(m_labels->rowCount(),
                        QFormLayout::SpanningRole,
                        m_beforeOpenWithLine);
    m_labels->addRow(m_openwithlabel, m_openwith);

    vb_main->addLayout(hl_files);
    vb_main->addWidget(p->makeLine(this));
    vb_main->addLayout(m_labels);
    vb_main->addItem(new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));

    QMimeDatabase m;
    auto mime = m.mimeTypeForFile(p->m_target);
    if(mime.name() == "inode/directory"
        || m_mountDevice)
        disableOpenWith();
}

void LSGeneralInfoWidget::reloadInfo(struct stat sb)
{
    QLocale l;
    QFileInfo f(p->m_target);
    if(m_mountDevice == nullptr)
    {
        auto pm = p->m_icons->icon(f);
        auto pixmap = pm.pixmap(64,64);
        pixmap = pixmap.scaled(40,40);
        m_pixmap->setPixmap(pixmap);
        m_filename->setText(f.fileName());

        auto filesize = l.formattedDataSize(sb.st_size).replace(' ', '\n');
        m_filesize->setText(filesize);

        auto disk_size = sb.st_blksize * sb.st_blocks;
        m_disksize->setText(l.formattedDataSize(disk_size));

        QMimeDatabase m;
        auto mime = m.mimeTypeForFile(p->m_target);
        m_fileinfo->setText(mime.comment());
        m_mimetype->setText(mime.name());
        m_location->setText(f.canonicalPath());

        QDateTime ctime = QDateTime::fromSecsSinceEpoch(sb.st_ctim.tv_sec);
        QDateTime mtime = QDateTime::fromSecsSinceEpoch(sb.st_mtim.tv_sec);
        QDateTime atime = QDateTime::fromSecsSinceEpoch(sb.st_atim.tv_sec);
        m_ctime->setText(l.toString(ctime));
        m_mtime->setText(l.toString(mtime));
        m_atime->setText(l.toString(atime));

        auto def = p->m_mime->defaultApp(mime.name());
        auto apps = p->m_mime->apps(mime.name());

        if(def)
            m_openwith->addItem(def->icon(), def->value("Name").toString());

        for(auto a : apps)
            m_openwith->addItem(a->icon(), a->value("Name").toString());
    }
    else
    {
        auto pm = m_mountDevice->icon();
        auto pixmap = pm.pixmap(64,64);
        pixmap = pixmap.scaled(40,40);
        m_pixmap->setPixmap(pixmap);
        m_filename->setText(m_mountDevice->name());
        m_filename->setEnabled(false);
        m_location->setText(m_mountDevice->mountpoint());
        m_fileinfo->setText(m_mountDevice->currentMediaDisplayName());
        m_mimetype->setText(m_mountDevice->devDevice());
        m_disksize->setText(m_mountDevice->filesystem());
        if(!m_mountDevice->mountpoint().isEmpty())
        {
            // if we are mounted we can use statvfs
            // to get our info:
            struct statvfs stat;
            if(statvfs(m_mountDevice->mountpoint().toUtf8().data(), &stat) == 0)
            {
                unsigned long totalSize = stat.f_frsize * stat.f_blocks;
                unsigned long usedSize = stat.f_frsize * (stat.f_blocks - stat.f_bfree);
                unsigned long freeSize = stat.f_frsize * stat.f_bfree;
                // capacity
                m_ctime->setText(l.formattedDataSize(totalSize));
                // available (free)
                m_mtime->setText(l.formattedDataSize(freeSize));
                // used
                m_atime->setText(l.formattedDataSize(usedSize));
            }
        }
        else
        {
            // not mounted - see what else we can do (comes in bytes)
            auto size = m_mountDevice->blockDeviceSize();
            m_ctime->setText(l.formattedDataSize(size));
        }
    }
}

void LSGeneralInfoWidget::disableOpenWith()
{
    m_beforeOpenWithLine->setVisible(false);
    m_openwithlabel->setVisible(false);
    m_openwith->setVisible(false);
}

void LSGeneralInfoWidget::setReadOnly()
{
    m_filename->setEnabled(false);
}

LSPermissionsAclWidget::LSPermissionsAclWidget(LSGetInfoDialogPrivate *parent)
    : QWidget(parent->d)
    , m_user(new QComboBox(this))
    , m_group(new QComboBox(this))
    , m_ur(new QCheckBox(this))
    , m_uw(new QCheckBox(this))
    , m_ux(new QCheckBox(this))
    , m_gr(new QCheckBox(this))
    , m_gw(new QCheckBox(this))
    , m_gx(new QCheckBox(this))
    , m_or(new QCheckBox(this))
    , m_ow(new QCheckBox(this))
    , m_ox(new QCheckBox(this))
    , m_acl(new QTreeView(this))
    , p(parent)
{
    auto vl_perms = new QVBoxLayout(this);
    auto gb_unix = new QGroupBox(QApplication::tr("UNIX permissions:"), this);
    auto gb_acl = new QGroupBox(QApplication::tr("Access control list:"), this);
    auto acl_layout = new QVBoxLayout(gb_acl);
    acl_layout->addWidget(m_acl);
    vl_perms->addWidget(gb_unix);
    vl_perms->addWidget(gb_acl);

    auto unix_hbox = new QHBoxLayout(gb_unix);

    auto vl_user = new QVBoxLayout;
    auto vl_group = new QVBoxLayout;
    auto vl_other = new QVBoxLayout;

    auto pm_user = new QLabel(m_acl);
    auto pm_group = new QLabel(m_acl);
    auto pm_others = new QLabel(m_acl);
    auto other_lbl = new QLabel(QApplication::tr("All Others"));

    pm_user->setPixmap(QIcon::fromTheme("user").pixmap(32,32));
    pm_group->setPixmap(QIcon::fromTheme("group").pixmap(32,32));
    pm_others->setPixmap(QIcon::fromTheme("user-others").pixmap(32,32));

    vl_user->addWidget(pm_user);
    vl_user->addWidget(m_user);
    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    vl_user->addWidget(line);
    vl_user->addWidget(m_ur);
    vl_user->addWidget(m_uw);
    vl_user->addWidget(m_ux);

    vl_group->addWidget(pm_group);
    vl_group->addWidget(m_group);
    auto line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    vl_group->addWidget(line2);
    vl_group->addWidget(m_gr);
    vl_group->addWidget(m_gw);
    vl_group->addWidget(m_gx);

    vl_other->addWidget(pm_others);
    vl_other->addWidget(other_lbl);
    auto line3 = new QFrame(this);
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    vl_other->addWidget(line3);
    vl_other->addWidget(m_or);
    vl_other->addWidget(m_ow);
    vl_other->addWidget(m_ox);

    unix_hbox->addLayout(vl_user);
    unix_hbox->addLayout(vl_group);
    unix_hbox->addLayout(vl_other);

    pm_user->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    pm_group->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    pm_others->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    other_lbl->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    m_ur->setText(QApplication::tr("Read"));
    m_gr->setText(QApplication::tr("Read"));
    m_or->setText(QApplication::tr("Read"));

    m_uw->setText(QApplication::tr("Write"));
    m_gw->setText(QApplication::tr("Write"));
    m_ow->setText(QApplication::tr("Write"));

    m_ux->setText(QApplication::tr("Execute"));
    m_gx->setText(QApplication::tr("Execute"));
    m_ox->setText(QApplication::tr("Execute"));

    loadNssPasswd();
    loadNssGroup();
}

void LSPermissionsAclWidget::reloadInfo(struct stat sb)
{
    struct passwd *pw = getpwuid(sb.st_uid);
    m_user->setCurrentText(pw->pw_name);

    struct group *g = getgrgid(sb.st_gid);
    m_group->setCurrentText(g->gr_name);

    QFile fi(p->m_target);
    auto perms = fi.permissions();
    m_ur->setChecked(perms & QFileDevice::ReadOwner);
    m_gr->setChecked(perms & QFileDevice::ReadGroup);
    m_or->setChecked(perms & QFileDevice::ReadOther);

    m_uw->setChecked(perms & QFileDevice::WriteOwner);
    m_gw->setChecked(perms & QFileDevice::WriteGroup);
    m_ow->setChecked(perms & QFileDevice::WriteOther);

    m_ux->setChecked(perms & QFileDevice::ExeOwner);
    m_gx->setChecked(perms & QFileDevice::ExeGroup);
    m_ox->setChecked(perms & QFileDevice::ExeOther);
}

void LSPermissionsAclWidget::setReadOnly()
{
    setEnabled(false);
}

void LSPermissionsAclWidget::loadNssPasswd()
{
    struct passwd *pw = nullptr;
    setpwent();

    for(;;)
    {
        pw = getpwent();
        if(pw == NULL)
            break;

        m_user->addItem(pw->pw_name, pw->pw_uid);
    }
    endpwent();
}

void LSPermissionsAclWidget::loadNssGroup()
{
    struct group *grp = nullptr;
    setgrent();

    for(;;)
    {
        grp = getgrent();
        if(grp == NULL)
            break;

        m_group->addItem(grp->gr_name, grp->gr_gid);
    }
    endgrent();
}

LSDesktopEntryInfoWidget::LSDesktopEntryInfoWidget(LSGetInfoDialogPrivate *parent)
    : QWidget(parent->d)
    , m_icon(new QToolButton(this))
    , m_name(new QLineEdit(this))
    , m_comment(new QLineEdit(this))
    , m_program(new QLineEdit(this))
    , m_arguments(new QLineEdit(this))
    , m_wd(new QLineEdit(this))
    , m_term(new QCheckBox(this))
    , m_dbus(new QCheckBox(this))
    , m_gpu(new QCheckBox(this))
    , m_sectionMimeTypes(new LSSectionWidget(tr("MIME Types"), 100, this))
    , m_mimeTypes(new QListWidget(m_sectionMimeTypes))
    , p(parent)
{
    m_icon->setMinimumSize(48,48);
    m_icon->setMaximumSize(48,48);
    m_icon->setIconSize(QSize(32,32));
    m_icon->setToolTip(tr("Select Icon"));
    m_icon->setAutoRaise(true);

    auto mainLayout = new QVBoxLayout(this);
    auto form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    setLayout(mainLayout);
    auto commentLabel = new QLabel(tr("Comment:"), this);
    auto programLabel = new QLabel(tr("Program:"), this);
    auto argumentLabel = new QLabel(tr("Arguments:"), this);
    auto workingDirectoryLabel = new QLabel(tr("Working Directory:"), this);
    auto optionsLabel = new QLabel(tr("Options:"), this);

    m_term->setText(tr("Launch in terminal"));
    m_dbus->setText(tr("Activate with DBus"));
    m_gpu->setText(tr("Start on discrete GPU"));

    auto toplayout = new QHBoxLayout;
    toplayout->addWidget(m_icon);
    toplayout->addWidget(m_name);

    auto mimelayout = new QVBoxLayout(this);
    mimelayout->addWidget(m_mimeTypes);
    m_sectionMimeTypes->setContentLayout(*mimelayout);

    form->addRow(p->makeLine(this));
    form->addRow(commentLabel, m_comment);
    form->addRow(programLabel, m_program);
    form->addRow(argumentLabel, m_arguments);
    form->addRow("", m_term);
    form->addRow(workingDirectoryLabel, m_wd);
    form->addRow(p->makeLine(this));
    form->addRow(optionsLabel, m_gpu);
    form->addRow("", m_dbus);

    mainLayout->addLayout(toplayout);
    mainLayout->addLayout(form);
    mainLayout->addWidget(m_sectionMimeTypes);
    /* QDialogButtonBox* buttonBar = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::SaveAs, this);
    mainLayout->addWidget(buttonBar, 6, 0, 1, 2); */
}

void LSDesktopEntryInfoWidget::reloadInfo()
{
    LSDesktopEntry desktop;
    desktop.load(p->m_target);
    if(desktop.isValid())
    {
        m_icon->setIcon(desktop.icon());
        m_name->setText(desktop.value("Name").toString());
        m_comment->setText(desktop.value("Comment").toString());
        m_program->setText(desktop.value("Exec").toString());
        m_wd->setText(desktop.value("Path").toString());
    }
}

void LSDesktopEntryInfoWidget::setReadOnly()
{
    setEnabled(false);
}

LSApplicationInfoWidget::LSApplicationInfoWidget(LSGetInfoDialogPrivate *parent)
    : QWidget(parent->d)
    , m_icon(new QLabel(this))
    , m_name(new QLabel(this))
    , m_uninstall(new QPushButton(this))
    , p(parent)
{
    m_icon->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    m_icon->setMinimumSize(48,48);
    m_icon->setMaximumSize(48,48);

    bool apk = false;
    bool flatpak = false;

    if(p->m_desktop_entry)
    {
        if(p->m_desktop_entry->checkForApk())
            apk = true;

        m_name->setText(p->m_desktop_entry->value("Name").toString());
        m_icon->setPixmap(p->m_desktop_entry->icon().pixmap(QSize(48,48)));
    }

    m_uninstall->setText(tr("Uninstall"));
    auto mainLayout = new QVBoxLayout(this);
    auto form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);
    setLayout(mainLayout);

    auto toplayout = new QHBoxLayout;
    toplayout->addWidget(m_icon);
    toplayout->addWidget(m_name);

    form->addRow(p->makeLine(this));
    if(apk)
    {
        auto apki = new QtApk::Database();
        apki->open();
        auto pkgs = apki->getInstalledPackages();
        for(auto pkg : pkgs)
        {
            if(pkg.name == p->m_desktop_entry->apkPackageName())
            {
                form->addRow(new QLabel(tr("Application Type:"), this),
                             new QLabel(tr("Distribution (APK)"), this));
                form->addRow(new QLabel(tr("Package Name:"), this),
                             new QLabel(pkg.name, this));
                form->addRow(new QLabel(tr("Version:"), this),
                             new QLabel(pkg.version, this));
                form->addRow(new QLabel(tr("License:"), this),
                             new QLabel(pkg.license, this));
                QLocale l;
                form->addRow(new QLabel(tr("Installed Size:"), this),
                             new QLabel(l.formattedDataSize(pkg.installedSize), this));

                if(pkg.name == "hollywood-desktop" || pkg.origin == "hollywood")
                    m_uninstall->setVisible(false);
                break;
            }
        }
        apki->close();
    }

    mainLayout->addLayout(toplayout);
    mainLayout->addLayout(form);

    auto buttons = new QHBoxLayout;
    buttons->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    buttons->addWidget(m_uninstall);
    buttons->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    mainLayout->addLayout(buttons);

    clearFocus();
    /* QDialogButtonBox* buttonBar = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::SaveAs, this);
    mainLayout->addWidget(buttonBar, 6, 0, 1, 2); */
}
