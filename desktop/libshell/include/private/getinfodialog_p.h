#ifndef GETINFODIALOG_P_H
#define GETINFODIALOG_P_H

#include <QUrl>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QWidget>
#include <QLineEdit>
#include <sys/stat.h>
#include <QTreeView>

#include "hwfileiconprovider.h"

class LSMimeApplications;
class LSGetInfoDialog;
class LSGetInfoDialogPrivate
{
    friend class LSGetInfoDialog;
    LSGetInfoDialogPrivate(LSGetInfoDialog *parent);
    ~LSGetInfoDialogPrivate() = default;
    LSGetInfoDialog *d;
    QFrame *makeLine();

    QUrl m_target;
    LSMimeApplications *m_mime;
    HWFileIconProvider *m_icons;
    bool m_init = true;
    ino_t m_inode;

    QWidget *m_info;
    QLabel *m_pixmap;
    QLineEdit *m_filename;
    QLabel *m_fileinfo;
    QLabel *m_filesize;
    QLabel *m_mimetype;

    QFormLayout *m_labels;
    QComboBox *m_openwith;

    QLabel *m_location;
    QLabel *m_disksize;
    QLabel *m_ctime;
    QLabel *m_mtime;
    QLabel *m_atime;

    QWidget *m_perms;
    QComboBox *m_user;
    QComboBox *m_group;

    QCheckBox *m_ur;
    QCheckBox *m_uw;
    QCheckBox *m_ux;
    QCheckBox *m_gr;
    QCheckBox *m_gw;
    QCheckBox *m_gx;
    QCheckBox *m_or;
    QCheckBox *m_ow;
    QCheckBox *m_ox;

    QTreeView *m_acl;

};

#endif // GETINFODIALOG_P_H
