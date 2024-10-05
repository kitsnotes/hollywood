// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QTreeView>
#include <QTextEdit>
#include <QLabel>
#include <QFormLayout>
#include <QToolButton>

class LSGetInfoDialogPrivate;

class LSGeneralInfoWidget : public QWidget
{
    Q_OBJECT
public:
    friend class LSGetInfoDialog;
    explicit LSGeneralInfoWidget(LSGetInfoDialogPrivate *parent);
    void reloadInfo(struct stat sb);
    void disableOpenWith();
    void setReadOnly();
private:
    QLabel *m_pixmap = nullptr;
    QLineEdit *m_filename = nullptr;
    QLabel *m_fileinfo = nullptr;
    QLabel *m_filesize = nullptr;
    QLabel *m_mimetype = nullptr;

    QFormLayout *m_labels = nullptr;
    QFrame *m_beforeOpenWithLine = nullptr;
    QLabel *m_openwithlabel = nullptr;
    QComboBox *m_openwith = nullptr;

    QLabel *m_location = nullptr;
    QLabel *m_disksize = nullptr;
    QLabel *m_ctime = nullptr;
    QLabel *m_mtime = nullptr;
    QLabel *m_atime = nullptr;
    LSGetInfoDialogPrivate *p;
};

class LSPermissionsAclWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LSPermissionsAclWidget(LSGetInfoDialogPrivate *parent);
    void reloadInfo(struct stat sb);
    void setReadOnly();
private:
    void loadNssPasswd();
    void loadNssGroup();
private:
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

    LSGetInfoDialogPrivate *p;
};

class LSDesktopEntryInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LSDesktopEntryInfoWidget(LSGetInfoDialogPrivate *parent);
    void reloadInfo();
    void setReadOnly();
private:
    QToolButton *m_icon = nullptr;
    QLineEdit* m_name = nullptr;
    QLineEdit* m_comment = nullptr;
    QLineEdit* m_program = nullptr;
    QLineEdit* m_arguments = nullptr;
    QLineEdit* m_wd = nullptr;
    QCheckBox* m_term = nullptr;
    QCheckBox* m_dbus = nullptr;
    QCheckBox* m_gpu = nullptr;

    //QDialogButtonBox* buttonBar = nullptr;

    LSGetInfoDialogPrivate *p;
};
