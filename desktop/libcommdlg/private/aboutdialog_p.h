#ifndef ABOUTDIALOG_P_H
#define ABOUTDIALOG_P_H

#include <QString>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>

class HWAboutDialog;
class AboutDialogPrivate
{
private:
    friend class HWAboutDialog;
    AboutDialogPrivate(HWAboutDialog *parent);

private:
    HWAboutDialog *d;
    QString m_name;
    QString m_version;
    QString m_desc;
    QString m_copyright;

    QVBoxLayout *layout;
    QLabel *m_iconLabel;
    QVBoxLayout *m_nameAboutLayout;
    QLabel *m_appName;
    QLabel *m_versionLabel;
    QSpacerItem *sp_Info;
    QLabel *m_infoLabel;
    QSpacerItem *sp_Copyright;
    QLabel *m_copyrightLabel;

};

#endif // ABOUTDIALOG_P_H
