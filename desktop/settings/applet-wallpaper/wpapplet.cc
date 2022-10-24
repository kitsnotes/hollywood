#include "wpapplet.h"
#include "chooser.h"

WallpaperApplet::WallpaperApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool WallpaperApplet::init()
{
    setupWidget();
    return true;
}

bool WallpaperApplet::loadSettings()
{
    return true;
}

bool WallpaperApplet::closeUp()
{
    delete m_host;
    m_host = nullptr;
    return true;
}

QString WallpaperApplet::id() const
{
    return QLatin1String("org.originull.wallpaper");
}

QString WallpaperApplet::name() const
{
    return tr("Wallpaper");
}

QString WallpaperApplet::description() const
{
    return tr("Configure desktop wallpaper.");
}

QIcon WallpaperApplet::icon() const
{
    return QIcon::fromTheme("preferences-desktop-wallpaper");
}

QWidget *WallpaperApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<WPChooserWidget*>(m_host);
}

SettingsAppletInterface::Category WallpaperApplet::category() const
{
    return Personal;
}

void WallpaperApplet::setupWidget()
{
    auto s = QApplication::primaryScreen();
    m_host = new WPChooserWidget(s,0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("Wallpaper"));
    m_host->resize(625, 550);
    m_host->setWindowTitle(tr("Wallpaper"));

    connect(m_host, &WPChooserWidget::wallpaperChanged,
            this, &WallpaperApplet::wallpaperChanged);

    // TODO: widgets in windows for foreign displays
}

void WallpaperApplet::wallpaperChanged()
{
    auto chooser = qobject_cast<WPChooserWidget*>(sender());
    QSettings settings("originull", "hollywood");
    if(chooser->primaryScreen())
        settings.beginGroup("PrimaryWallpaper");
    else
        settings.beginGroup(QString("Wallpaper-").arg(chooser->screen()->name()));

    settings.setValue("BackgroundColor", chooser->selectedColor());
    settings.setValue("Wallpaper", chooser->selectedWallpaperFile());
    settings.setValue("DisplayMode", chooser->displayMode());
    settings.setValue("Rotate", chooser->rotate());
    settings.setValue("RotateTime", chooser->rotateTime());
    settings.setValue("RotateMode", chooser->rotateMode());
    settings.setValue("RotateFolder", chooser->selectedFolder());
    settings.endGroup();
}

