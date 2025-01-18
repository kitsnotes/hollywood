#include "platformtheme.h"
#include <QtGui/private/qguiapplication_p.h>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QMimeType>
#include <QMimeDatabase>
#include <QFileInfo>
#include <QGuiApplication>
#include <QSettings>
#include <QApplication>
#include <QStyle>
#include <QWidget>
#include <private/qiconloader_p.h>
#include <QGuiApplication>
#include <qpa/qplatformintegration.h>
#include <dialoghelpers.h>
#include <QStyleHints>

#include <hollywood/hollywood.h>
#include "wayland.h"
#include "qdbusmenubar_p.h"

using namespace QtGSettings;

static bool isDbusMenuAvailable()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QString registrarService = QStringLiteral("com.canonical.AppMenu.Registrar");
    bool available = connection.interface()->isServiceRegistered(registrarService);
    return available;
}

static bool isDBusGlobalMenuAvailable()
{
    static bool dbusGlobalMenuAvailable = isDbusMenuAvailable();
    return dbusGlobalMenuAvailable;
}

HollywoodPlatformTheme::HollywoodPlatformTheme()
{
    auto val = qgetenv("HW_TWILIGHT_SHELL");
    if(!val.isEmpty())
    {
        qDebug() << "dark mode enabled for twilight shell";
        m_twilightShell = true;
    }

    m_appearanceSettings.reset(new QGSettings("org.originull.hollywood.appearance", "/org/originull/hollywood/appearance/", this));
    loadAppearanceSettings();
    m_inputSettings.reset(new QGSettings("org.originull.hollywood.input", "/org/originull/hollywood/input/", this));
    loadInputSettings();
    createPalettes();
    QMetaObject::invokeMethod(this, "secondInit", Qt::QueuedConnection);
}

bool HollywoodPlatformTheme::usePlatformNativeDialog(DialogType type) const
{
    if(type == MessageDialog)
        return true;

    return false;
}

QPlatformDialogHelper *HollywoodPlatformTheme::createPlatformDialogHelper(DialogType type) const
{
    if(type == MessageDialog)
    {
        auto dlg = new HWMessageDialogHelper;
        return dlg;
    }

    return nullptr;
}


void HollywoodPlatformTheme::windowCreated(QWindow *window)
{
    auto data = qgetenv("HOLLYWOOD_MS_INIT");

    if(!data.isEmpty())
        return;

    if (!DBusMenuBar::globalMenuBar())
        return;

    if (DBusMenuBar::menuBarForWindow(window))
        return;

    const QString &serviceName = QDBusConnection::sessionBus().baseService();
    const QString &objectPath = DBusMenuBar::globalMenuBar()->objectPath();

    setMenuBarForWindow(window, serviceName, objectPath);
}

#if QT_VERSION >= 0x060500
Qt::ColorScheme HollywoodPlatformTheme::colorScheme() const
{
    //0x0001 == light, 0x0002 == dark
    if(m_appearance == SetLight)
        return Qt::ColorScheme::Light;

    if(m_appearance == SetDark)
        return Qt::ColorScheme::Dark;

    if(m_appearance == SetTwilight)
    {
        if(m_twilightShell)
            return Qt::ColorScheme::Dark;
        else
            return Qt::ColorScheme::Light;
    }
    return Qt::ColorScheme::Unknown;
}

void HollywoodPlatformTheme::requestColorScheme(Qt::ColorScheme scheme)
{
    //qDebug() << "client requestColorScheme" << scheme;
    //m_customAppearanceMode = true;
    //m_customAppearanceRequest = scheme;
}
#endif

void HollywoodPlatformTheme::secondInit()
{
    QCoreApplication::instance()->installEventFilter(this);

    QTimer *timer = new QTimer(this);
      timer->setSingleShot(true);

      auto data = qgetenv("HOLLYWOOD_MS_INIT");

      if(data.isEmpty())
      {
          connect(timer, &QTimer::timeout, [=]() {
              if (QGuiApplication::platformName() == QLatin1String("wayland") ||
                  QGuiApplication::platformName() == QLatin1String("hollywood"))
                      m_wayland.reset(new WaylandIntegration(this));

              // in this timeout to give time for application to set
              loadDesktopFileSettings();

              connect(m_appearanceSettings.data(), &QGSettings::settingChanged,
                      this, &HollywoodPlatformTheme::appearanceSettingChanged);
              connect(m_inputSettings.data(), &QGSettings::settingChanged,
                      this, &HollywoodPlatformTheme::inputSettingChanged);
             timer->deleteLater();
          } );

        timer->start(150);
      }
    // using QWaylandClientExtension here still crashes
    // can we use a timer?

}

#if QT_VERSION > 0x060000 && QT_VERSION < 0x060500
QPlatformTheme::Appearance HollywoodPlatformTheme::appearance() const
{
    //0x0001 == light, 0x0002 == dark
    if(m_appearance == SetLight)
        return QPlatformTheme::Appearance::Light;

    if(m_appearance == SetDark)
        return QPlatformTheme::Appearance::Dark;

    if(m_appearance == SetTwilight)
    {
        if(m_twilightShell)
            return QPlatformTheme::Appearance::Dark;
        else
            return QPlatformTheme::Appearance::Light;
    }
    return QPlatformTheme::Appearance::Unknown;
}
#endif

const QPalette *HollywoodPlatformTheme::palette(Palette type) const
{
    return preferredPalette();
    if(type == QPlatformTheme::SystemPalette)
        return preferredPalette();

    return nullptr;
}

const QFont *HollywoodPlatformTheme::font(Font type) const
{
    QString family = QString(m_def_font);
    if(type == FixedFont)
        family = QString(m_fixed_sys);
    uint ps = HOLLYWOOD_PT_NORMAL;

    switch(m_fontSize)
    {
    case Small:
        ps = HOLLYWOOD_PT_SMALL;
        break;
    case Large:
        ps = HOLLYWOOD_PT_LARGE;
        break;
    case XLarge:
        ps = HOLLYWOOD_PT_XLARGE;
        break;
    case Normal:
    default:
        ps = HOLLYWOOD_PT_NORMAL;
    }

    if(type == TitleBarFont ||
       type == MdiSubWindowTitleFont ||
       type == DockWidgetTitleFont ||
       type == ToolButtonFont)
        ps = ps-2;

    auto font = new QFont(family,ps);
    return font;
}


QPalette *HollywoodPlatformTheme::preferredPalette() const
{
    if(m_appearance == SetLight)
        return m_palette_light;

    if(m_appearance == SetDark)
        return m_palette_dark;

    if(m_appearance == SetTwilight)
    {
        if(m_twilightShell)
            return m_palette_dark;

        return m_palette_light;
    }

    return nullptr;
}

QString HollywoodPlatformTheme::preferredStyle() const
{
    return QLatin1String(HOLLYWOOD_DEF_STYLE);
}

void HollywoodPlatformTheme::createPalettes()
{
    delete m_palette_light;

    m_palette_light = new QPalette(QStringLiteral(HOLLYWOOD_WNDCLR_LIGHT));
    QColor base(QStringLiteral(HOLLYWOOD_VIEWCLR_LIGHT));
    m_palette_light->setColor(QPalette::Base, base);
    int l = base.lightness();
    if (l < 127)
        l += 10;
    else
        l -= 10;
    base.setHsl(base.hue(), base.saturation(), l);
    m_palette_light->setColor(QPalette::AlternateBase, base);

    QColor ac = m_customAccent ? m_customAccentColor : m_accentColor;
    m_palette_light->setColor(QPalette::Highlight, ac);
    int gray = qGray(ac.rgb());
    ac = QColor(gray, gray, gray);
    m_palette_light->setColor(QPalette::Disabled, QPalette::Highlight, ac);

    QColor tc = QColor(QLatin1String(HOLLYWOOD_TEXTCLR_LIGHT));
    m_palette_light->setColor(QPalette::WindowText, tc);
    m_palette_light->setColor(QPalette::ButtonText, tc);
    m_palette_light->setColor(QPalette::Text, tc);
    tc.setAlpha(130);
    m_palette_light->setColor(QPalette::Disabled, QPalette::WindowText, tc);
    m_palette_light->setColor(QPalette::Disabled, QPalette::ButtonText, tc);
    m_palette_light->setColor(QPalette::Disabled, QPalette::Text, tc);

    QColor seltc = QColor(QLatin1String(HOLLYWOOD_SELTXT_LIGHT));
    m_palette_light->setColor(QPalette::HighlightedText, seltc);
    seltc.setAlpha(130);
    m_palette_light->setColor(QPalette::Disabled, QPalette::HighlightedText, seltc);
    m_palette_light->setColor(QPalette::Link, ac);

    //TODO: fix link visit
    m_palette_light->setColor(QPalette::LinkVisited, QColor(Qt::magenta));

    // Setup Dark Palette

    delete m_palette_dark;
    m_palette_dark = new QPalette(QStringLiteral(HOLLYWOOD_WNDCLR_DARK));
    QColor baseDark(QStringLiteral(HOLLYWOOD_VIEWCLR_DARK));
    m_palette_dark->setColor(QPalette::Base, baseDark);
    l = baseDark.lightness();
    if (l < 127)
        l += 10;
    else
        l -= 10;
    baseDark.setHsl(baseDark.hue(), baseDark.saturation(), l);
    m_palette_dark->setColor(QPalette::AlternateBase, baseDark);

    ac = m_customAccent ? m_customAccentColor : m_accentColor;
    m_palette_dark->setColor(QPalette::Highlight, ac);
    gray = qGray(ac.rgb());
    ac = QColor(gray, gray, gray);
    m_palette_dark->setColor(QPalette::Disabled, QPalette::Highlight, ac);

    tc = QColor(QLatin1String(HOLLYWOOD_TEXTCLR_DARK));
    m_palette_dark->setColor(QPalette::WindowText, tc);
    m_palette_dark->setColor(QPalette::ButtonText, tc);
    m_palette_dark->setColor(QPalette::Text, tc);
    tc.setAlpha(130);
    m_palette_dark->setColor(QPalette::Disabled, QPalette::WindowText, tc);
    m_palette_dark->setColor(QPalette::Disabled, QPalette::ButtonText, tc);
    m_palette_dark->setColor(QPalette::Disabled, QPalette::Text, tc);

    seltc = QColor(QLatin1String(HOLLYWOOD_SELTXT_DARK));
    m_palette_dark->setColor(QPalette::HighlightedText, seltc);
    seltc.setAlpha(130);
    m_palette_dark->setColor(QPalette::Disabled, QPalette::HighlightedText, seltc);
    m_palette_dark->setColor(QPalette::Link, ac);

    //TODO: fix link visit
    m_palette_light->setColor(QPalette::LinkVisited, QColor(Qt::magenta));

}

QString HollywoodPlatformTheme::preferredIcons() const
{
    if(m_appearance == SetDark)
        return QLatin1String(HOLLYWOOD_DARK_ICONTHEME);

    if(m_appearance == SetTwilight && m_twilightShell)
        return QLatin1String(HOLLYWOOD_DARK_ICONTHEME);

    return QLatin1String(HOLLYWOOD_DEF_ICONTHEME);
}

void HollywoodPlatformTheme::setMenuBarForWindow(QWindow *window, const QString &serviceName, const QString &objectPath) const
{
    if (!window)
        return;

    if(m_twilightShell)
    {
        if (m_wayland)
            m_wayland->setAppMenu(window, serviceName, objectPath);
    }
    /*if (m_x11Integration) {
        m_x11Integration->setWindowProperty(window, s_x11AppMenuServiceNamePropertyName, serviceName.toUtf8());
        m_x11Integration->setWindowProperty(window, s_x11AppMenuObjectPathPropertyName, objectPath.toUtf8());
    }*/
}

void HollywoodPlatformTheme::loadAppearanceSettings()
{
    auto _app = m_appearanceSettings->value(QLatin1String("appearanceMode")).toString();
    m_appearance = (SettingAppearance)0;

    if(_app == "dark")
        m_appearance = (SettingAppearance)1;

    if(_app == "twilight")
        m_appearance = (SettingAppearance)2;

    auto accent = m_appearanceSettings->value(QLatin1String("accentColor")).toString();
    QColor ac = QColor(accent);
    if(!ac.isValid())
        ac = QColor(QLatin1String(HOLLYWOOD_ACCENT_BLUE));
    m_accentColor = ac;

    auto fontszstr = m_appearanceSettings->value(QLatin1String("fontSize")).toString();
    m_fontSize = (FontSize)1;
    if(fontszstr == "small")
        m_fontSize = (FontSize)0;
    if(fontszstr == "large")
        m_fontSize = (FontSize)2;
    if(fontszstr == "xlarge")
        m_fontSize = (FontSize)3;

    m_customAccent = m_appearanceSettings->value("allowAccentOverride").toBool();

    m_def_font = m_appearanceSettings->value("defaultFontName").toString();
    m_fixed_sys = m_appearanceSettings->value("monospaceFontName").toString();

}

void HollywoodPlatformTheme::loadInputSettings()
{
    m_wheelScroll = m_inputSettings->value("mouseScrollSpeed").toUInt();
    m_doubleClickInt = m_inputSettings->value("mouseDoubleClickSpeed").toUInt();

    QApplication::setDoubleClickInterval(m_doubleClickInt);
}

void HollywoodPlatformTheme::loadDesktopFileSettings()
{
    if(!QGuiApplication::desktopFileName().isEmpty())
    {
        auto dt = findDesktopFile();
        if(!dt.isEmpty())
        {
            QSettings settings(dt, QSettings::IniFormat);
            settings.beginGroup("Desktop Entry");
            auto color = settings.value("X-Hollywood-AccentColor").toString();
            if(!color.isEmpty())
            {
                QColor test(color);
                if(test.isValid())
                {
                    m_customAccent = true;
                    m_customAccentColor = QColor(color);
                    createPalettes();
                    auto palette = preferredPalette();
                    if(auto app = qobject_cast<QGuiApplication *>(QCoreApplication::instance()))
                    {
                        if(palette != nullptr)
                        {
                            QGuiApplication::setPalette(*palette);
                        }
                        const auto windows = QGuiApplication::allWindows();
                        for(QWindow* const window : windows)
                        {
                            QEvent themeEvent(QEvent::ThemeChange);
                            QGuiApplication::sendEvent(window, &themeEvent);
                            QEvent styleEvent(QEvent::StyleChange);
                            QGuiApplication::sendEvent(window, &styleEvent);
                        }
                    }
                    if(auto app = qobject_cast<QApplication *>(QCoreApplication::instance()))
                    {
                        if(palette != nullptr)
                        {
                            QApplication::style()->polish(app);
                        }
                        const auto widgets = QApplication::allWidgets();
                        for(QWidget* const widget : widgets)
                        {
                            QEvent themeEvent(QEvent::ThemeChange);
                            QGuiApplication::sendEvent(widget, &themeEvent);
                            QEvent styleEvent(QEvent::StyleChange);
                            QGuiApplication::sendEvent(widget, &styleEvent);
                        }
                    }
                }
            }
        }
    }
}

QStringList HollywoodPlatformTheme::dataDirs()
{
    QString d = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));
    QStringList dirs = d.split(QLatin1Char(':'), Qt::SkipEmptyParts);
    QStringList cleaned;

    if (dirs.isEmpty()) {
        cleaned.append(QString::fromLatin1("/usr/local/share"));
        cleaned.append(QString::fromLatin1("/usr/share"));
    } else {
        for(auto dir : dirs)
        {
            if(!dir.startsWith(QLatin1Char('/')))
                continue;

            // the platform theme does not work with flatpak apps
            // so let's not target a .desktop in that
            if(dir.contains("flatpak"))
                continue;

            if(dir.endsWith(QLatin1Char('/')))
                cleaned.append(dir.removeLast());
            else
                cleaned.append(dir);
        }
    }

    return cleaned;
}

QString HollywoodPlatformTheme::findDesktopFile()
{
    for(auto dirname : dataDirs())
    {
        QDir dir(QString("%1/applications").arg(dirname));
        if(dir.exists())
        {
            auto files = dir.entryList(QStringList() << "*.desktop");
            for(auto file : files)
            {
                if(file == QApplication::desktopFileName() + ".desktop")
                    return QString(dirname + "/applications/" + file);
            }
        }
    }

    return QString();
}

void HollywoodPlatformTheme::appearanceSettingChanged(const QString &key)
{
    if(!QGuiApplication::desktopSettingsAware())
        return;

    bool fontChange = false;
    bool repolish = true;

    auto oldIconTheme = preferredIcons();
    auto oldFont = QApplication::font().family();
    auto changeFontName = oldFont;
    auto oldFontSize = m_fontSize;
    auto old_dark = m_appearance;
    auto old_custom_accent = m_customAccent;

    if(key == "accentColor")
    {
        auto accent = m_appearanceSettings->value(QLatin1String("accentColor")).toString();
        QColor ac = QColor(accent);
        if(!ac.isValid())
            ac = QColor(QLatin1String(HOLLYWOOD_ACCENT_BLUE));
        m_accentColor = ac;
        repolish = true;
    }
    if(key == "appearanceMode")
    {
        auto _app = m_appearanceSettings->value(QLatin1String("appearanceMode")).toString();
        m_appearance = (SettingAppearance)0;

        if(_app == "dark")
            m_appearance = (SettingAppearance)1;

        if(_app == "twilight")
            m_appearance = (SettingAppearance)2;
        repolish = true;
    }
    if(key == "allowAccentOverride")
    {
        m_customAccent = m_appearanceSettings->value("allowAccentOverride").toBool();
        if(m_customAccent != old_custom_accent)
            repolish = true;
    }
    if(key == "fontSize")
    {
        auto fontszstr = m_appearanceSettings->value(QLatin1String("fontSize")).toString();
        m_fontSize = (FontSize)1;
        if(fontszstr == "small")
            m_fontSize = (FontSize)0;
        if(fontszstr == "large")
            m_fontSize = (FontSize)2;
        if(fontszstr == "xlarge")
            m_fontSize = (FontSize)3;
        fontChange = true;
        repolish = true;
    }
    if(key == "defaultFontName")
    {
        auto newfont = m_appearanceSettings->value(QLatin1String("defaultFontName")).toString();
        if(newfont != oldFont)
        {
            changeFontName = newfont;
            fontChange = true;
        }
    }

    if(fontChange)
    {
        QFont font(QGuiApplication::font());
        bool dochg = false;
        if(changeFontName != oldFont)
            dochg = true;

        if(m_fontSize != oldFontSize)
        {
            switch(m_fontSize)
            {
            case Small:
                font.setPointSize(HOLLYWOOD_PT_SMALL);
                break;
            case Large:
                font.setPointSize(HOLLYWOOD_PT_LARGE);
                break;
            case XLarge:
                font.setPointSize(HOLLYWOOD_PT_XLARGE);
                break;
            case Normal:
            default:
                font.setPointSize(HOLLYWOOD_PT_NORMAL);
                break;
            }
            dochg = true;
        }
        if(dochg)
        {
            font.setFamily(changeFontName);
            QGuiApplication::setFont(font);
            auto app = qobject_cast<QApplication*>(qApp);
            if(app != nullptr)
                app->setFont(font);
        }
    }
    //auto oldIconTheme = preferredIcons();

    if(repolish)
    {
        if(old_dark != m_appearance)
        {
            if(m_appearance == SetLight)
                QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
            else if(m_appearance == SetDark)
                QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
            else if(m_appearance == SetTwilight)
            {
                if(m_twilightShell)
                    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
                else
                    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
            }
        }
        createPalettes();
        auto palette = preferredPalette();
        if(preferredIcons() != oldIconTheme)
            QIconLoader::instance()->updateSystemTheme();
        if(auto app = qobject_cast<QApplication *>(QCoreApplication::instance()))
        {
            if(palette != nullptr)
            {
                QGuiApplication::setPalette(*palette);
                QApplication::style()->polish(app);
            }
        }
    }

    // emit a ThemeChange event to all widgets
    const auto widgets = QApplication::allWidgets();
    for(QWidget* const widget : widgets)
    {
        if(fontChange)
        {
            QEvent fontEvent(QEvent::FontChange);
            QApplication::sendEvent(widget, &fontEvent);
        }
        if(repolish)
        {
            QEvent themeEvent(QEvent::ThemeChange);
            QEvent styleEvent(QEvent::StyleChange);
            QEvent repolishEvent(QEvent::PolishRequest);

            QApplication::sendEvent(widget, &themeEvent);
            QApplication::sendEvent(widget, &styleEvent);
            QApplication::sendEvent(widget, &repolishEvent);
        }
    }
}

void HollywoodPlatformTheme::inputSettingChanged(const QString &key)
{
    if(!QGuiApplication::desktopSettingsAware())
        return;

    if(key == "mouseScrollSpeed")
    {
        m_wheelScroll = m_inputSettings->value("mouseScrollSpeed").toUInt();
        QApplication::setWheelScrollLines(m_wheelScroll);
    }
    if(key == "mouseDoubleClickSpeed")
    {
        m_doubleClickInt = m_inputSettings->value("mouseDoubleClickSpeed").toUInt();
        QApplication::setDoubleClickInterval(m_doubleClickInt);
    }
}

QVariant HollywoodPlatformTheme::themeHint(ThemeHint hint) const {
    switch (hint) {
    case CursorFlashTime:
        return m_cursorFlash;
    case SystemIconThemeName:
        return preferredIcons();
    case WheelScrollLines:
        return m_wheelScroll;
    case StyleNames:
        return QStringList() << preferredStyle();
    case ItemViewActivateItemOnSingleClick:
        return QVariant(m_singleClickActivate);
    case MouseDoubleClickInterval:
        return m_doubleClickInt;
    //case ToolButtonStyle:
    //    return QVariant(toolButtonStyle_);
    case SystemIconFallbackThemeName:
        return QLatin1String("hicolor");
    case DropShadow:
        return QVariant(true);
    case DialogButtonBoxButtonsHaveIcons:
        return QVariant(false);
    case KeyboardScheme:
        return QVariant(MacKeyboardScheme);
#if QT_VERSION >= 0x060000
    case IconThemeSearchPaths:
        return xdgIconThemePaths();
#endif
    case QPlatformTheme::ShowShortcutsInContextMenus:
        return QVariant(true);
#if QT_VERSION < 0x060000
    case TabAllWidgets:
#endif
    case UnderlineShortcut:
        return QVariant(false);
    case KeyboardInputInterval:
    case StartDragDistance:
    case StartDragTime:
    case KeyboardAutoRepeatRate:
    case PasswordMaskDelay:
    case StartDragVelocity:
    case TextCursorWidth:
    case MaximumScrollBarDragDistance:
    case ToolBarIconSize:
    case WindowAutoPlacement:
    case DialogButtonBoxLayout:
    case UseFullScreenForPopupMenu:
    case UiEffects:
    case SpellCheckUnderlineStyle:
    case IconPixmapSizes:
    case PasswordMaskCharacter:
    case DialogSnapToDefaultButton:
    case ContextMenuOnMouseRelease:
    case MousePressAndHoldInterval:
    case MouseDoubleClickDistance:
    default:
        break;
    }
    return QPlatformTheme::themeHint(hint);
}


QPlatformTheme* HollywoodPlatformThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    if(key.toLower() == "hollywood")
        return new HollywoodPlatformTheme;

    return nullptr;
}

QIcon HollywoodPlatformTheme::fileIcon(const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions) const
{
    if((iconOptions & DontUseCustomDirectoryIcons) && fileInfo.isDir())
        return QIcon::fromTheme(QLatin1String("inode-directory"));

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(fileInfo);
    auto mtname = type.name();
    mtname.replace('/', '-');
    return QIcon::fromTheme(mtname);
}

inline bool windowRelevantForGlobalMenu(QWindow* window)
{
    return !(window->type() & Qt::WindowType::Popup);
}

void HollywoodPlatformTheme::globalMenuBarExistsNow()
{
    const QString &serviceName = QDBusConnection::sessionBus().baseService();
    const QString &objectPath = DBusMenuBar::globalMenuBar()->objectPath();

    for (auto *window : qApp->topLevelWindows()) {
        if (DBusMenuBar::menuBarForWindow(window))
            continue;
        if (!windowRelevantForGlobalMenu(window))
            return;

        setMenuBarForWindow(window, serviceName, objectPath);
    }
}

void HollywoodPlatformTheme::globalMenuBarNoLongerExists()
{
    for (auto *window : qApp->topLevelWindows())
    {
        if (DBusMenuBar::menuBarForWindow(window))
            continue;
        if (!windowRelevantForGlobalMenu(window))
            return;

        setMenuBarForWindow(window, {}, {});
    }
}

QPlatformMenuBar *HollywoodPlatformTheme::createPlatformMenuBar() const
{
    if (isDBusGlobalMenuAvailable())
    {
        auto *menu = new DBusMenuBar(const_cast<HollywoodPlatformTheme *>(this));

        QObject::connect(menu, &DBusMenuBar::windowChanged, menu, [this, menu](QWindow *newWindow, QWindow *oldWindow) {
            const QString &serviceName = QDBusConnection::sessionBus().baseService();
            const QString &objectPath = menu->objectPath();

            setMenuBarForWindow(oldWindow, {}, {});
            setMenuBarForWindow(newWindow, serviceName, objectPath);
        });

        return menu;
    }

    return nullptr;
}


