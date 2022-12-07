#ifndef APPMODEL_P_H
#define APPMODEL_P_H

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

#endif // APPMODEL_P_H
