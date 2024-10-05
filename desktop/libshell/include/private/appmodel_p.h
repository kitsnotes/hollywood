#pragma once

#include <QList>

class LSMimeApplications;
class LSDesktopEntry;
class ApplicationModel;
class AppModelPrivate
{
private:
    friend class ApplicationModel;
    AppModelPrivate(ApplicationModel *parent);
    ~AppModelPrivate();
    LSMimeApplications *m_mimeapps;
    QList<LSDesktopEntry *> m_allApps;
    ApplicationModel *d;
};
