#ifndef LSMIMEAPPS_H
#define LSMIMEAPPS_H
/*
 * derived from the implementation in libqtxdg
 * Copyright (C) 2018  Luís Pereira <luis.artur.pereira@gmail.com>
 */

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QFileSystemWatcher>
#include <QHash>
#include <QMultiHash>

#include "libshell_int.h"

class LSDesktopEntry;
class LIBSHELL_EXPORT LSMimeApplications : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LSMimeApplications)
public:
    explicit LSMimeApplications(QObject *parent = nullptr);
    ~LSMimeApplications() override;
    bool addSupport(const QString &mimeType, const LSDesktopEntry &app);
    QList<LSDesktopEntry *> allApps();
    QList<LSDesktopEntry *> apps(const QString &mimeType);
    QList<LSDesktopEntry *> categoryApps(const QString &category);
    QList<LSDesktopEntry *> fallbackApps(const QString &mimeType);

    QList<LSDesktopEntry *> recommendedApps(const QString &mimeType);
    LSDesktopEntry *defaultApp(const QString &mimeType);
    bool removeSupport(const QString &mimeType, const LSDesktopEntry &app);
    bool reset(const QString &mimeType);
    bool setDefaultApp(const QString &mimeType, const LSDesktopEntry &app);
    LSDesktopEntry* findDesktopForFile(const QString &file);
    LSDesktopEntry* findDesktopForExec(const QString &file);
    void cacheAllDesktops();
public slots:
    void processGlobalMimeCache();
signals:
    void changed();
private slots:
    void processDefaults();
    void fileUpdated(const QString &file);
private:
    QMultiHash<QString,LSDesktopEntry*> m_globalMime;
    QMultiHash<QString,LSDesktopEntry*> m_localMime;
    QHash<QString,LSDesktopEntry*> m_defaults;

    QList<LSDesktopEntry*> m_desktops;
    QFileSystemWatcher *m_watch = nullptr;
    QMutex m_mutex;
    QString m_defaultsFile;
    QString m_localMimeCacheFile;
};

#endif // LSMIMEAPPS_H
