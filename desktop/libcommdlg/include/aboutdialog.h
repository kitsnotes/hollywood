#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "libcommdlg_global.h"

//typedef ADAboutDialog LSAboutDialog;
class LIBCOMMDLG_EXPORT ADAboutDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString appName READ appName WRITE setAppName NOTIFY appNameChanged);
    Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged);
    Q_PROPERTY(QString applicationDescription READ appDescription WRITE setAppDescription NOTIFY appDescriptionChanged);
    Q_PROPERTY(QString appCopyright READ appCopyright WRITE setAppCopyright NOTIFY appCopyrightChanged);
public:
    ADAboutDialog(QWidget *parent = 0);
    QString appName() const;
    QString appVersion() const;
    QString appDescription() const;
    QString appCopyright() const;
public slots:
    void setAppName(const QString &name);
    void setAppVersion(const QString &version);
    void setAppDescription(const QString &description);
    void setAppCopyright(const QString &copyright);
signals:
    void appNameChanged(const QString &name);
    void appVersionChanged(const QString &version);
    void appDescriptionChanged(const QString &description);
    void appCopyrightChanged(const QString &copyright);
private:
    void setupDialog();
private:
    QString m_name;
    QString m_version;
    QString m_desc;
    QString m_copyright;
private:
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

#endif // ABOUTDIALOG_H
