#ifndef USERSAPPLET_H
#define USERSAPPLET_H

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
#include <QList>
#include <QListView>
#include <QToolButton>
#include <Qt6AccountsService/AccountsManager>
#include <Qt6AccountsService/UserAccount>
#include <Qt6AccountsService/UsersModel>
#include <QIdentityProxyModel>
#include <settingsdelegate.h>

#define HW_APP_HELP_TOPIC "settings/60-users"

class UsersModel : public QtAccountsService::UsersModel
{
    Q_OBJECT
public:
    UsersModel(QObject *parent = nullptr): QtAccountsService::UsersModel(parent) {}

    QVariant data(const QModelIndex &index, int role) const;
};

class UsersGroupsApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit UsersGroupsApplet(QObject *parent = nullptr);
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
private:
    void setupWidget();
    void setupAccountService();
    void updateViewForUser(QtAccountsService::UserAccount *user);
    void setUserIcon(QtAccountsService::UserAccount *user);

    QWidget* m_host = nullptr;
    QVBoxLayout *mainLayout = nullptr;

    QListView *m_users = nullptr;
    QToolButton *m_icon = nullptr;
    QLabel *m_dname = nullptr;

    QPushButton *m_chgpass = nullptr;
    QPushButton *m_startup = nullptr;

    QLabel *m_linuxname = nullptr;
    QLineEdit *e_dname = nullptr;
    QLineEdit *e_fname = nullptr;
    QLineEdit *e_email = nullptr;

    QCheckBox *m_admin = nullptr;
    QCheckBox *m_autologin = nullptr;

    QToolButton *m_help = nullptr;

    QtAccountsService::AccountsManager *m_m = nullptr;
    UsersModel *m_model = nullptr;

    SettingsSidebarDelegate *m_delegate = nullptr;
};

#endif // APKAPPLET_H
