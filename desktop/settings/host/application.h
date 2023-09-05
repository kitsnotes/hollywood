#ifndef SETTINGSAPPLICATION_H
#define SETTINGSAPPLICATION_H

#include <QApplication>
#include <QObject>
#include <QDBusAbstractAdaptor>
#include <hollywood/hollywood.h>

class SettingsWindow;
class SettingsApplication : public QApplication
{
    Q_OBJECT
public:
    SettingsApplication(int &argc, char **argv);
    static SettingsApplication* instance() { return static_cast<SettingsApplication*>(QApplication::instance()); }
    bool registerDBus();
    void showMainWindow(const QString &prefpane);
    void showSingleWindow(const QString &prefpane);
protected:
    friend class SettingsDBus;
    void activate();
    void openPane(const QString &prefpane);
private:
    void createMainWindow();
    SettingsWindow *m_mainWnd = nullptr;
};

class SettingsDBus : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Application")

public:
    SettingsDBus(SettingsApplication *parent)
        : QDBusAbstractAdaptor(parent)
        , m_parent(parent) {}
public slots:
    Q_NOREPLY void Activate(const QVariantMap &platform_data)
    {
        Q_UNUSED(platform_data)
        m_parent->activate();
    }
    Q_NOREPLY void Open(const QStringList &uris, const QVariantMap &platform_data)
    {
        Q_UNUSED(platform_data)
        if(uris.isEmpty())
            m_parent->activate();
        else
            m_parent->openPane(uris.first());
    }
    Q_NOREPLY void ActivateAction(const QString &action_name, const QString &parameter,
                                  const QVariantMap &platform_data)
    {
        Q_UNUSED(platform_data)
        Q_UNUSED(action_name)
        Q_UNUSED(parameter)
    }
private:
    SettingsApplication *m_parent;
};



#endif // SETTINGSAPPLICATION_H
