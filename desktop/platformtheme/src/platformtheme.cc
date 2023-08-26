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

#include <hollywood/hollywood.h>
#include "wayland.h"
#include "qdbusmenubar_p.h"

static bool isDbusMenuAvailable()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QString registrarService = QStringLiteral("com.canonical.AppMenu.Registrar");
    bool available = connection.interface()->isServiceRegistered(registrarService);
    //qDebug() << "testing if dbusmenu available: " << available;
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
        m_twilightShell = true;

    loadSettings();
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

void HollywoodPlatformTheme::secondInit()
{
    QCoreApplication::instance()->installEventFilter(this);
    m_configwatch = new QFileSystemWatcher();
    m_configwatch->addPath(m_configfile);
    connect(m_configwatch, &QFileSystemWatcher::fileChanged,
            this, &HollywoodPlatformTheme::settingsChanged);

    QTimer *timer = new QTimer(this);
      timer->setSingleShot(true);

      auto data = qgetenv("HOLLYWOOD_MS_INIT");

      if(data.isEmpty())
      {
          connect(timer, &QTimer::timeout, [=]() {
              if (QGuiApplication::platformName() == QLatin1String("wayland") ||
                  QGuiApplication::platformName() == QLatin1String("hollywood"))
                      m_wayland.reset(new WaylandIntegration(this));
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
    if(m_apperance == SetLight)
        return QPlatformTheme::Appearance::Light;

    if(m_apperance == SetDark)
        return QPlatformTheme::Appearance::Dark;

    if(m_apperance == SetTwilight)
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


    //qDebug() << family << ps;
    auto font = new QFont(family,ps);
    return font;
}


QPalette *HollywoodPlatformTheme::preferredPalette() const
{
    if(m_apperance == SetLight)
        return m_palette_light;

    if(m_apperance == SetDark)
        return m_palette_dark;

    if(m_apperance == SetTwilight)
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

    auto ac = m_accentColor;
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
    m_palette_light->setColor(QPalette::Link, m_accentColor);

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

    ac = m_accentColor;
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
    m_palette_dark->setColor(QPalette::Link, m_accentColor);

    //TODO: fix link visit
    m_palette_light->setColor(QPalette::LinkVisited, QColor(Qt::magenta));

}

QString HollywoodPlatformTheme::preferredIcons() const
{
    if(m_apperance == SetDark)
        return QLatin1String(HOLLYWOOD_DARK_ICONTHEME);

    if(m_apperance == SetTwilight && m_twilightShell)
        return QLatin1String(HOLLYWOOD_DARK_ICONTHEME);

    return QLatin1String(HOLLYWOOD_DEF_ICONTHEME);
}

void HollywoodPlatformTheme::setMenuBarForWindow(QWindow *window, const QString &serviceName, const QString &objectPath) const
{
    if (!window)
        return;

    auto data = qgetenv("HOLLYWOOD_MS_INIT");

    if(data.isEmpty())
    {
        if (m_wayland)
            m_wayland->setAppMenu(window, serviceName, objectPath);
    }
    /*if (m_x11Integration) {
        m_x11Integration->setWindowProperty(window, s_x11AppMenuServiceNamePropertyName, serviceName.toUtf8());
        m_x11Integration->setWindowProperty(window, s_x11AppMenuObjectPathPropertyName, objectPath.toUtf8());
    }*/
}

void HollywoodPlatformTheme::settingsChanged()
{
    bool file_deleted = !m_configwatch->files().contains(m_configfile);
    if(file_deleted)
        m_configwatch->addPath(m_configfile);

    auto oldIconTheme = preferredIcons();
    SettingApperance oldApp = m_apperance;
    QColor oldac = m_accentColor;
    FontSize oldFont = m_fontSize;
    loadSettings();
    createPalettes();

    QApplication::setWheelScrollLines(m_wheelScroll);
    auto palette = preferredPalette();

    if(oldApp != m_apperance || oldac != m_accentColor ||
            m_paletteChanged) // the widget style or palette is changed
    {
        if(auto app = qobject_cast<QApplication *>(QCoreApplication::instance()))
        {
            if(palette != nullptr)
            {
                QGuiApplication::setPalette(*palette);
                QApplication::style()->polish(app);
            }
        }
    }

    if(preferredIcons() != oldIconTheme)
        QIconLoader::instance()->updateSystemTheme();

    // check for font changes
    QFont font(QApplication::font());
    bool font_change = false;
    if(m_fontSize != oldFont)
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
        font_change = true;
    }

    bool is_eglfs = false;

    if(QGuiApplication::platformName() == "eglfs" ||
            QGuiApplication::platformName() == "hollywood-eglfs")
        is_eglfs = true;

    if(font_change && !is_eglfs)
        QApplication::setFont(font);

    // emit a ThemeChange event to all widgets
    const auto widgets = QApplication::allWidgets();
    for(QWidget* const widget : widgets)
    {
        QEvent themeEvent(QEvent::ThemeChange);
        QEvent fontEvent(QEvent::FontChange);
        QApplication::sendEvent(widget, &themeEvent);
        QApplication::sendEvent(widget, &fontEvent);
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
        return QVariant(false);
#if QT_VERSION < 0x060000
    case TabAllWidgets:
#endif
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
        qDebug() << "creating new DBus Menubar";
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


void HollywoodPlatformTheme::loadSettings()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));
    m_configfile = settings.fileName();

    settings.beginGroup(QLatin1String("General"));
    uint _app = settings.value(QLatin1String("ApperanceMode")).toUInt();
    if(_app > 2)
        _app = 0;   // default to light mode on invalid settings
    m_apperance = (SettingApperance)_app;

    auto accent = settings.value(QLatin1String("AccentColor")).toString();
    QColor ac = QColor(accent);
    if(!ac.isValid())
        ac = QColor(QLatin1String(HOLLYWOOD_ACCENT_BLUE));
    m_accentColor = ac;

    auto fontsz = settings.value(QLatin1String("FontSize"), 1).toUInt();
    if(fontsz > 3)
        fontsz = 1;

    m_fontSize = (FontSize)fontsz;

    m_def_font = settings.value("DefaultFont", HOLLYWOOD_DEF_STDFONT).toString();
    m_fixed_sys = settings.value("MonospaceFont", HOLLYWOOD_DEF_FIXEDSYS).toString();

    settings.endGroup();

    settings.beginGroup(QLatin1String("Mouse"));
    auto ws = settings.value("MouseScrollSpeed", HOLLYWOOD_DEF_SCROLL_SPEED).toUInt();
    if(ws > HOLLYWOOD_MAX_SCROLL_SPEED)
        ws = HOLLYWOOD_MAX_SCROLL_SPEED;
    if(ws <= 1)
        ws = 1;
    m_wheelScroll = ws;

    auto dblclk = settings.value("DoubleClickSpeed", HOLLYWOOD_DEF_DBLCLK_TIME).toUInt();
    if(dblclk > HOLLYWOOD_MAX_DBLCLK_TIME)
        dblclk = HOLLYWOOD_MAX_DBLCLK_TIME;
    if(dblclk < HOLLYWOOD_MIN_DBLCLK_TIME)
        dblclk = HOLLYWOOD_MIN_DBLCLK_TIME;

    m_doubleClickInt = dblclk;
    settings.endGroup();
}

