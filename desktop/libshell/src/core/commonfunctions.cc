#include "commonfunctions.h"
#include <QDBusInterface>
#include <QDBusMessage>
#include <hollywood/hollywood.h>

#include "getinfodialog.h"

LSCommonFunctions *g_common_func = nullptr;

LSCommonFunctions* LSCommonFunctions::instance()
{
    if(g_common_func == nullptr)
        g_common_func = new LSCommonFunctions();

    return g_common_func;
}

bool LSCommonFunctions::executeDesktopOverDBus(const QString &desktop)
{
    QDBusInterface ldb(HOLLYWOOD_SESSION_DBUS, HOLLYWOOD_SESSION_DBUSOBJ, HOLLYWOOD_SESSION_DBUSPATH);
    if(!ldb.isValid())
    {
        qDebug() << QString("Could not call session DBUS interface. Command: executeDesktop");
        return false;
    }

    QDBusMessage msg = ldb.call("executeDesktop", desktop, QStringList(), QStringList());

    if(msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
        return true;

    auto response = msg.arguments().constFirst();

    if(msg.arguments().isEmpty())
        return true;

    if(response.isNull())
        return true;

    if(response.toBool())
        return true;

    return false;
}

bool LSCommonFunctions::openFileOverDBusWithDefault(const QString &file)
{
    QDBusInterface ldb(HOLLYWOOD_SESSION_DBUS, HOLLYWOOD_SESSION_DBUSOBJ, HOLLYWOOD_SESSION_DBUSPATH);
    if(!ldb.isValid())
    {
        qDebug() << QString("Could not call session DBUS interface. Command: openFileWithDefault");
        return false;
    }

    QDBusMessage msg = ldb.call("openFileWithDefault", file);

    if(msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
        return true;

    auto response = msg.arguments().constFirst();

    if(msg.arguments().isEmpty())
        return true;

    if(response.isNull())
        return true;

    if(response.toBool())
        return true;

    return false;
}

void LSCommonFunctions::showGetInfoDialog(const QUrl &target, QWidget *parent)
{
    auto dlg = new LSGetInfoDialog(target, parent);
    connect(dlg, &QDialog::accepted, dlg, &QWidget::deleteLater);
    connect(dlg, &QDialog::rejected, dlg, &QWidget::deleteLater);
    dlg->open();
}
