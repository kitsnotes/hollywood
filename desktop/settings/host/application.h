#ifndef SETTINGSAPPLICATION_H
#define SETTINGSAPPLICATION_H

#include <QApplication>
#include <QObject>

class SettingsApplication : public QApplication
{
    Q_OBJECT
public:
    SettingsApplication(int &argc, char **argv);
    static SettingsApplication* instance() { return static_cast<StageApplication*>(QApplication::instance()); }

private:
};

#endif // SETTINGSAPPLICATION_H
