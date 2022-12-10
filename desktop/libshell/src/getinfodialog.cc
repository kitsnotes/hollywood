#include "getinfodialog.h"
#include "getinfodialog_p.h"

#include <QApplication>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QGroupBox>

#include "mimeapps.h"
#include "desktopentry.h"

#include <pwd.h>
#include <grp.h>

LSGetInfoDialogPrivate::LSGetInfoDialogPrivate(LSGetInfoDialog *parent)
    : d(parent)
    , m_mime(new LSMimeApplications)
    , m_icons(new HWFileIconProvider)
    , m_info(new QWidget(parent))
    , m_pixmap(new QLabel(m_info))
    , m_filename(new QLineEdit(m_info))
    , m_fileinfo(new QLabel(m_info))
    , m_filesize(new QLabel(m_info))
    , m_mimetype(new QLabel(m_info))
    , m_labels(new QFormLayout)
    , m_openwith(new QComboBox(m_info))
    , m_location(new QLabel(m_info))
    , m_disksize(new QLabel(m_info))
    , m_ctime(new QLabel(m_info))
    , m_mtime(new QLabel(m_info))
    , m_atime(new QLabel(m_info))
    , m_perms(new QWidget(parent))
    , m_user(new QComboBox(m_perms))
    , m_group(new QComboBox(m_perms))
    , m_ur(new QCheckBox(m_perms))
    , m_uw(new QCheckBox(m_perms))
    , m_ux(new QCheckBox(m_perms))
    , m_gr(new QCheckBox(m_perms))
    , m_gw(new QCheckBox(m_perms))
    , m_gx(new QCheckBox(m_perms))
    , m_or(new QCheckBox(m_perms))
    , m_ow(new QCheckBox(m_perms))
    , m_ox(new QCheckBox(m_perms))
    , m_acl(new QTreeView(m_perms))
{
    auto font = m_filesize->font();
    font.setPointSize(font.pointSize()+1);
    m_filesize->setFont(font);
    m_fileinfo->setIndent(15);
    m_filesize->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    m_pixmap->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    // setup the main view
    auto vb_main = new QVBoxLayout(m_info);

    auto hl_files = new QHBoxLayout;
    auto vl_filename = new QVBoxLayout;
    vl_filename->addWidget(m_filename);
    vl_filename->addWidget(m_fileinfo);
    hl_files->addWidget(m_pixmap);
    hl_files->addLayout(vl_filename);
    hl_files->addWidget(m_filesize);

    auto l_loc = new QLabel(QApplication::tr("Location:"), m_info);
    auto l_ds = new QLabel(QApplication::tr("Size on disk:"), m_info);
    auto l_mt = new QLabel(QApplication::tr("MIME type:"), m_info);
    auto l_ct = new QLabel(QApplication::tr("Created:"), m_info);
    auto l_mod = new QLabel(QApplication::tr("Modified:"), m_info);
    auto l_at = new QLabel(QApplication::tr("Accessed:"), m_info);
    auto l_ow = new QLabel(QApplication::tr("Open with:"), m_info);

    m_labels->setLabelAlignment(Qt::AlignRight);
    m_labels->addRow(l_loc, m_location);
    m_labels->addRow(l_mt, m_mimetype);
    m_labels->addRow(l_ds, m_disksize);
    m_labels->addRow(l_ct, m_ctime);
    m_labels->addRow(l_mod, m_mtime);
    m_labels->addRow(l_at, m_atime);
    m_labels->setWidget(m_labels->rowCount(), QFormLayout::SpanningRole, makeLine());
    m_labels->addRow(l_ow, m_openwith);

    vb_main->addLayout(hl_files);
    vb_main->addWidget(makeLine());
    vb_main->addLayout(m_labels);
    vb_main->addItem(new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));

    auto vl_perms = new QVBoxLayout(m_perms);
    auto gb_unix = new QGroupBox(QApplication::tr("UNIX permissions:"), m_perms);
    auto gb_acl = new QGroupBox(QApplication::tr("Access control list:"), m_perms);
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
    vl_user->addWidget(makeLine());
    vl_user->addWidget(m_ur);
    vl_user->addWidget(m_uw);
    vl_user->addWidget(m_ux);

    vl_group->addWidget(pm_group);
    vl_group->addWidget(m_group);
    vl_group->addWidget(makeLine());
    vl_group->addWidget(m_gr);
    vl_group->addWidget(m_gw);
    vl_group->addWidget(m_gx);

    vl_other->addWidget(pm_others);
    vl_other->addWidget(other_lbl);
    vl_other->addWidget(makeLine());
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

}

QFrame *LSGetInfoDialogPrivate::makeLine()
{
    auto line = new QFrame(d);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    return line;
}

LSGetInfoDialog::LSGetInfoDialog(const QUrl &file, QWidget *parent)
    : HWPreferenceDialog(parent)
    , p(new LSGetInfoDialogPrivate(this))
{
    p->m_target = file;
    setFocusPolicy(Qt::ClickFocus);
    setMaximumWidth(425);
    setMinimumWidth(425);
    setWindowTitle(QApplication::tr("File Information - %1").arg(file.fileName()));
    addPage(p->m_info, QIcon::fromTheme("kt-info-widget"), tr("Information"));
    addPage(p->m_perms, QIcon::fromTheme("lock"), tr("Permissions"));
    setCurrentIndex(0);
    reloadInfo();
}

void LSGetInfoDialog::reloadInfo()
{
    auto file = p->m_target.path();

    struct stat sb;
    if(stat(file.toUtf8(), &sb) == -1)
    {
        // TODO: handle failure;
    }
    if(!p->m_init)
    {
        p->m_inode = sb.st_ino;
        p->m_init = true;
    }

    QFileInfo f(file);
    auto pm = p->m_icons->icon(f);
    auto pixmap = pm.pixmap(64,64);
    pixmap = pixmap.scaled(40,40);
    p->m_pixmap->setPixmap(pixmap);
    p->m_filename->setText(f.fileName());

    QLocale l;
    auto filesize = l.formattedDataSize(sb.st_size).replace(' ', '\n');
    p->m_filesize->setText(filesize);

    auto disk_size = sb.st_blksize * sb.st_blocks;
    p->m_disksize->setText(l.formattedDataSize(disk_size));

    QMimeDatabase m;
    auto mime = m.mimeTypeForFile(file);
    p->m_fileinfo->setText(mime.comment());
    p->m_mimetype->setText(mime.name());
    p->m_location->setText(f.canonicalPath());

    QDateTime ctime = QDateTime::fromSecsSinceEpoch(sb.st_ctim.tv_sec);
    QDateTime mtime = QDateTime::fromSecsSinceEpoch(sb.st_mtim.tv_sec);
    QDateTime atime = QDateTime::fromSecsSinceEpoch(sb.st_atim.tv_sec);
    p->m_ctime->setText(l.toString(ctime));
    p->m_mtime->setText(l.toString(mtime));
    p->m_atime->setText(l.toString(atime));

    auto def = p->m_mime->defaultApp(mime.name());
    auto apps = p->m_mime->apps(mime.name());

    if(def)
    {
        qDebug() << def->value("Name").toString();
        p->m_openwith->addItem(def->icon(), def->value("Name").toString());
    }
    for(auto a : apps)
        p->m_openwith->addItem(a->icon(), a->value("Name").toString());

    struct passwd *pw = getpwuid(sb.st_uid);
    p->m_user->addItem(pw->pw_name);
    p->m_user->setCurrentText(pw->pw_name);

    struct group *g = getgrgid(sb.st_gid);
    p->m_group->addItem(g->gr_name);
    p->m_group->setCurrentText(g->gr_name);

}

void LSGetInfoDialog::open()
{
    HWPreferenceDialog::open();
    p->m_filename->clearFocus();
    p->m_openwith->clearFocus();
}
