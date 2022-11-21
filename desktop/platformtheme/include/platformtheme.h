#ifndef PLATFORMTHEME_H
#define PLATFORMTHEME_H

#include <QObject>
#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <QFont>
#include <QColor>
#include <QFileSystemWatcher>
#include <qpa/qplatformtheme.h>
#include <qpa/qplatformthemeplugin.h>
#if QT_VERSION >= 0x060000
#include <private/qgenericunixthemes_p.h>
#endif
#include "wayland.h"

class HollywoodPlatformThemePlugin : public QPlatformThemePlugin {
    Q_OBJECT
#if QT_VERSION < 0x060000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "platformtheme.json")
#else
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "platformtheme.json")
#endif
public:
    QPlatformTheme* create(const QString &key, const QStringList &params) override;
};

#if QT_VERSION < 0x060000
class HollywoodPlatformTheme : public QObject, public QPlatformTheme
#else
class HollywoodPlatformTheme : public QObject, public QGenericUnixTheme
#endif
{
    Q_OBJECT
public:
    explicit HollywoodPlatformTheme();
    // Reflect enums in generalapplet.h
    enum SettingApperance {
        SetLight,
        SetDark,
        SetTwilight,
        SetAutomatic
    };
    enum FontSize {
        Small,
        Normal,
        Large,
        XLarge
    };

    //bool usePlatformNativeDialog(DialogType type) const override;
    //QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const override;
    QPlatformMenuBar *createPlatformMenuBar() const override;

    void globalMenuBarExistsNow();
    void globalMenuBarNoLongerExists();
    void windowCreated(QWindow *window);

#if QT_VERSION > 0x060000
    Appearance appearance() const override;
#endif
    const QPalette* palette(Palette type = SystemPalette) const override;
    const QFont* font(Font type = SystemFont) const override;
    QVariant themeHint(ThemeHint hint) const override;
    QIcon fileIcon(const QFileInfo &fileInfo,
                   QPlatformTheme::IconOptions = { }) const override;
public slots:
    void secondInit();
private:
    void loadSettings();
    void settingsChanged();
    QPalette *preferredPalette() const;
    QString preferredStyle() const;
    void createPalettes();
    QString preferredIcons() const;
    void setMenuBarForWindow(QWindow* window, const QString& serviceName, const QString& objectPath) const;
private:
    QFileSystemWatcher *m_configwatch = nullptr;
    QString m_configfile;

    SettingApperance m_apperance = SetLight;
    bool m_paletteChanged = false;
    QPalette *m_palette_light = nullptr;
    QPalette *m_palette_dark = nullptr;

    FontSize m_fontSize;
    QString m_def_font, m_fixed_sys;

    QString m_iconTheme;
    QColor m_accentColor;
    bool m_singleClickActivate = false;
    bool m_iconColorize = true;

    uint m_doubleClickInt;
    uint m_wheelScroll;
    uint m_cursorFlash = 1000;

    QScopedPointer<WaylandIntegration> m_wayland;

    bool m_twilightShell = false;
};

#endif // PLATFORMTHEME_H
