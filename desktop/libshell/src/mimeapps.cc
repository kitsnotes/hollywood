#include "mimeapps.h"
#include "mimeapps_p.h"
#include "desktopentry.h"
#include "directories.h"

#include <QByteArray>
#include <QString>

LSMimeApplicationsPrivate::LSMimeApplicationsPrivate(LSMimeApplications *parent)
    : d(parent)
    , m_watch(new QFileSystemWatcher(parent)) {}

LSMimeApplications::LSMimeApplications(QObject *parent)
    : QObject(parent)
    , p(new LSMimeApplicationsPrivate(this))
{
    // setup our inotify watcher and add our files
    connect(p->m_watch, &QFileSystemWatcher::fileChanged,
            this, &LSMimeApplications::fileUpdated);
    // watch for defaults changes ~/.config/mimeapps.list
    QFile file(QString("%1/%2").arg(LOCAL_CONFIG_FOLDER).arg(DEFAULTS_LIST));
    p->m_defaultsFile = file.fileName();
    if(file.exists())
        p->m_watch->addPath(file.fileName());

    // watch for global mime app assoication changes
    // from /usr/share/applications/mimeinfo.cache
    QFile mimedb(QString("%1/%2").arg(GLOBAL_APPS_FOLDER).arg(MIMEINFO_CACHE));
    p->m_defaultsFile = file.fileName();
    if(file.exists())
        p->m_watch->addPath(file.fileName());

    // from ~/.local/share/appliications/mimeinfo.cache
    // watch for local mime app assoication changes
    QFile lmimedb(QString("%1/%2").arg(LOCAL_APPS_FOLDER).arg(MIMEINFO_CACHE));
    p->m_defaultsFile = file.fileName();
    if(file.exists())
        p->m_watch->addPath(file.fileName());

}

LSMimeApplications::~LSMimeApplications()
{
    delete p;
}

bool LSMimeApplications::addSupport(const QString &mimeType, const LSDesktopEntry &app)
{
    if (mimeType.isEmpty() || !app.isValid())
        return false;

    QMutexLocker locker(&p->m_mutex);
    // TODO: fill out this function
    return false;
}

QList<LSDesktopEntry *> LSMimeApplications::allApps()
{
    QList<LSDesktopEntry *> list;
    QMutexLocker locker(&p->m_mutex);
    list = p->m_desktops;
    return list;
}

QList<LSDesktopEntry *> LSMimeApplications::apps(const QString &mimeType)
{
    if (mimeType.isEmpty())
        return QList<LSDesktopEntry *>();

    QList<LSDesktopEntry *> list;
    QMutexLocker locker(&p->m_mutex);
    return list;
}

QList<LSDesktopEntry *> LSMimeApplications::categoryApps(const QString &category)
{
    if (category.isEmpty())
        return QList<LSDesktopEntry *>();

    const QString cat = category.toUpper();
    const QList <LSDesktopEntry *> apps = allApps();
    QList <LSDesktopEntry *> dl;
    for (LSDesktopEntry * const df : apps) {
        const QStringList categories = df->value(QLatin1String("Categories")).toString()
                .toUpper().split(QLatin1Char(';'));
        if (!categories.isEmpty() && (categories.contains(cat)
            || categories.contains(QLatin1String("X-") + cat)))
            dl.append(df);
        else
            delete df;
    }
    return dl;
}

QList<LSDesktopEntry *> LSMimeApplications::fallbackApps(const QString &mimeType)
{
    if (mimeType.isEmpty())
        return QList<LSDesktopEntry *>();

    QMutexLocker locker(&p->m_mutex);
    QList<LSDesktopEntry *> list;
    return list;
}

QList<LSDesktopEntry *> LSMimeApplications::recommendedApps(const QString &mimeType)
{
    if (mimeType.isEmpty())
        return QList<LSDesktopEntry *>();

    QMutexLocker locker(&p->m_mutex);
    QList<LSDesktopEntry *> list;
    return list;
}

LSDesktopEntry *LSMimeApplications::defaultApp(const QString &mimeType)
{
    if (mimeType.isEmpty())
        return nullptr;

    QMutexLocker locker(&p->m_mutex);

    // return our user preference (if set)
    if(p->m_defaults.contains(mimeType))
        return p->m_defaults[mimeType];

    // no user preference? find the first global app
    if(p->m_globalMime.contains(mimeType))
    {
        qDebug() << "returning global value" << p->m_globalMime.values(mimeType).first()->fileName();
        return p->m_globalMime.values(mimeType).first();
    }

    return nullptr;
}

bool LSMimeApplications::removeSupport(const QString &mimeType, const LSDesktopEntry &app)
{
    if (mimeType.isEmpty() || !app.isValid())
        return false;

    QMutexLocker locker(&p->m_mutex);
    return false;
}

bool LSMimeApplications::reset(const QString &mimeType)
{
    if (mimeType.isEmpty())
        return false;

    QMutexLocker locker(&p->m_mutex);
    return false;
}

bool LSMimeApplications::setDefaultApp(const QString &mimeType, const LSDesktopEntry &app)
{
    if (mimeType.isEmpty() || !app.isValid())
        return false;

    if (LSDesktopEntry::id(app.fileName()).isEmpty())
        return false;

    QMutexLocker locker(&p->m_mutex);
    return false;
}

void LSMimeApplications::fileUpdated(const QString &file)
{
    if(file == QString("%1/%2").arg(GLOBAL_APPS_FOLDER).arg(MIMEINFO_CACHE))
        processGlobalMimeCache();

    if(file == p->m_defaultsFile)
        processDefaults();
}

void LSMimeApplications::processGlobalMimeCache()
{
    for(auto ls : p->m_globalMime.values())
    {
        p->m_desktops.removeOne(ls);
        delete ls;
    }

    p->m_globalMime.clear();

    QString fileName = QString("%1/%2")
            .arg(GLOBAL_APPS_FOLDER)
            .arg(MIMEINFO_CACHE);

    QFile file(fileName);
    if(file.exists())
    {
        QSettings settings(fileName, QSettings::IniFormat);
        settings.beginGroup(QLatin1String("MIME Cache"));
        for(auto mime : settings.allKeys())
        {
            QStringList candidates = settings.value(mime).toStringList();
            for(auto candidate : candidates)
            {
                qDebug() << mime << candidate;
                auto filename = LSDesktopEntry::findDesktopFile(candidate);
                auto cache = findDesktopForFile(filename);
                if(cache == nullptr)
                {
                    auto desktop = new LSDesktopEntry();
                    desktop->load(filename);
                    p->m_globalMime.insert(mime, desktop);
                    p->m_desktops.append(desktop);
                }
                else
                    p->m_globalMime.insert(mime, cache);
            }
        }
    }
}

void LSMimeApplications::processDefaults()
{
    for(auto ls : p->m_defaults.values())
    {
        p->m_desktops.removeOne(ls);
        delete ls;
    }

    p->m_defaults.clear();

    QString fileName = QString("%1/%2")
            .arg(GLOBAL_APPS_FOLDER)
            .arg(MIMEINFO_CACHE);

    QFile file(fileName);
    if(file.exists() && file.isReadable())
    {
        QSettings settings(fileName, QSettings::IniFormat);
        settings.beginGroup(QLatin1String("Default Applications"));
        for(auto mime : settings.allKeys())
        {
            // we need to see if this is a global or local file
            // global takes presidence
            auto desktopPath = settings.value(mime).toString();
            auto filename = LSDesktopEntry::findDesktopFile(desktopPath);
            if(QFile::exists(filename))
            {
                auto cache = findDesktopForFile(filename);
                if(cache == nullptr)
                {
                    auto desktop = new LSDesktopEntry();
                    desktop->load(filename);
                    p->m_defaults.insert(mime, desktop);
                    p->m_desktops.append(desktop);
                }
                else
                    p->m_defaults.insert(mime, cache);
            }
        }
    }
}

LSDesktopEntry *LSMimeApplications::findDesktopForFile(const QString &file)
{
    for(auto desktop : p->m_desktops)
    {
        if(desktop->fileName() == file)
            return desktop;
    }

    return nullptr;
}

LSDesktopEntry *LSMimeApplications::findDesktopForExec(const QString &file)
{
    // this function can pass either a full path or a binary name
    // this will check just for a binary name
    for(auto desktop : p->m_desktops)
    {
        if(desktop->value("Exec") == file)
            return desktop;
    }

    // nothing found? ok let's see if the full path matches something
    // in PATH env and then re-query the exe name if so....
    auto keys = file.split('/');
    if(keys.count() > 0)
    {
        auto exe = keys.last();
        keys.pop_back();
        auto path = keys.join('/');
        // see if we have presence in PATH
        auto paths = qgetenv("PATH").split(':');
        if(paths.contains(path.toUtf8()))
            return findDesktopForExec(exe);
    }
    return nullptr;
}

void LSMimeApplications::cacheAllDesktops()
{
    QStringList dataDirs = LSDirectories::dataDirs();
    dataDirs.prepend(LSDirectories::dataHome(false));

    for(auto dirname : dataDirs)
    {
        QDir dir(QString("%1/applications").arg(dirname));
        if(dir.exists())
        {
            auto files = dir.entryList(QStringList() << "*.desktop");
            for(auto file : files)
            {
                auto entry = new LSDesktopEntry;
                entry->load(file);
                if(entry->isValid())
                    p->m_desktops.append(entry);
            }
        }
    }
}
