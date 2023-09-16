#ifndef LSSETTINGSAPPLETINTERFACE_H
#define LSSETTINGSAPPLETINTERFACE_H

#include <QtPlugin>

class QString;
class QIcon;
class QWidget;
class SettingsAppletInterface
{
public:
    enum Category {
        Personal,
        System,
        Other
    };
    virtual ~SettingsAppletInterface() = default;

    virtual bool available() const = 0;
    virtual bool init() = 0;
    virtual bool closeUp() = 0;
    virtual bool loadSettings() = 0;
    virtual bool saveSettings() = 0;
    virtual bool canExit() const = 0;
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QIcon icon() const = 0;
    virtual QWidget* applet() const = 0;
    virtual Category category() const = 0;
    virtual QString helpTopic() const = 0;
    virtual QStringList searchTokens() const = 0;
};

#define SettingsAppletInterfaceIID "org.orignull.settings-applet/1.0"
Q_DECLARE_INTERFACE(SettingsAppletInterface, SettingsAppletInterfaceIID)
#endif // LSSETTINGSAPPLETINTERFACE_H
