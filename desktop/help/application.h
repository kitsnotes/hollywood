#ifndef SETTINGSAPPLICATION_H
#define SETTINGSAPPLICATION_H

#include <QApplication>
#include <QObject>
#include <QProxyStyle>
#include <QDBusAbstractAdaptor>
#include <hollywood/hollywood.h>

#define HW_DEFAULT_HELP "/usr/share/qhelp/org.originull.hollywood.qch"
#define HW_GUIDE_URN "org.originull.hollywood.edge"

class HelpWindow;
class HelpProxyStyle : public QProxyStyle
{
public:
    int styleHint(StyleHint hint, const QStyleOption *option = nullptr,
                  const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override
    {

        if(hint == QStyle::SH_ItemView_ActivateItemOnSingleClick)
            return 1;

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

class HelpViewerApp : public QApplication
{
    Q_OBJECT
public:
    HelpViewerApp(int &argc, char **argv);
    static HelpViewerApp* instance() { return static_cast<HelpViewerApp*>(QApplication::instance()); }
    bool registerDBus();
    void showMainWindow(const QUrl &url);
protected:
    friend class SettingsDBus;
    void activate();
    void openUrl(const QUrl &url);
    void openIndex();
private:
    void createMainWindow();
    HelpWindow *m_mainWnd = nullptr;
};

class SettingsDBus : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Application")

public:
    SettingsDBus(HelpViewerApp *parent)
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
        //if(uris.isEmpty())
            m_parent->activate();
        //else
        //    m_parent->openPane(uris.first());
    }
    Q_NOREPLY void ActivateAction(const QString &action_name, const QString &parameter,
                                  const QVariantMap &platform_data)
    {
        Q_UNUSED(platform_data)
        Q_UNUSED(action_name)
        Q_UNUSED(parameter)
    }
private:
    HelpViewerApp *m_parent;
};



#endif // SETTINGSAPPLICATION_H
