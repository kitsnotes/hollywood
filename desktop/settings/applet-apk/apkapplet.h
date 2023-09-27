#ifndef APKAPPLET_H
#define APKAPPLET_H

#include <hollywood/appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QDBusInterface>
#include <QProgressBar>
#include <QtApkDatabaseAsync.h>

#define HW_APP_HELP_TOPIC "settings/25-swupdate"

class APKUpdateApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit APKUpdateApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings() { return true; }
    bool saveSettings() { return true; }
    bool canExit() const { return true; }
    bool closeUp() { delete m_host; m_host = nullptr; return true; }
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
    QString helpTopic() const  { return QString(HW_APP_HELP_TOPIC); }
    QStringList searchTokens() const;
private slots:
    void transactionFinished();
    void transactionProgress(double progress);
    void transactionMessage(QString message);
    void startUpdates();
    void doUpdateCheck();
private:
    void setupWidget();
    void checkForUpdates();
    bool isCurrentlyRunning();
    bool connectForUpdates();
    QWidget* m_host = nullptr;
    QVBoxLayout *mainLayout = nullptr;

    QLabel *lb_updates;
    QLabel *lb_desc;
    QPushButton *m_update;
    QPushButton *m_details;

    QProgressBar *m_progress = nullptr;
    QDBusInterface* m_dbus;
};



#endif // APKAPPLET_H
