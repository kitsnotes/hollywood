#pragma once

#include <QList>
#include <QMutex>
#include <QFileSystemWatcher>

class LSMimeApplications;
class LSDesktopEntry;
class LSMimeApplicationsPrivate
{
private:
    friend class LSMimeApplications;
    LSMimeApplications *d;
    LSMimeApplicationsPrivate(LSMimeApplications *parent);
    ~LSMimeApplicationsPrivate() = default;
    QMultiHash<QString,LSDesktopEntry*> m_globalMime;
    QMultiHash<QString,LSDesktopEntry*> m_localMime;
    QHash<QString,LSDesktopEntry*> m_defaults;

    QList<LSDesktopEntry*> m_desktops;
    QFileSystemWatcher *m_watch = nullptr;
    QMutex m_mutex;
    QString m_defaultsFile;
    QString m_localMimeCacheFile;
};
