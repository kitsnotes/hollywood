#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "libcommdlg_global.h"
#include <QDialog>

class AboutDialogPrivate;
class LIBCOMMDLG_EXPORT HWAboutDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString appName READ appName WRITE setAppName NOTIFY appNameChanged)
    Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged)
    Q_PROPERTY(QString applicationDescription READ appDescription WRITE setAppDescription NOTIFY appDescriptionChanged)
    Q_PROPERTY(QString appCopyright READ appCopyright WRITE setAppCopyright NOTIFY appCopyrightChanged)
public:
    HWAboutDialog(QWidget *parent = 0);
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
    AboutDialogPrivate *q;
};

#endif // ABOUTDIALOG_H
