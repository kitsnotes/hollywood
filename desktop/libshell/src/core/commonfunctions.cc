// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "commonfunctions.h"
#include "commonfunctions_p.h"
#include <QDBusInterface>
#include <QDBusMessage>
#include <QMessageBox>
#include <hollywood/hollywood.h>
#include <QList>
#include <QMimeData>
#include <QClipboard>

#include "getinfodialog.h"
#include "opmanager.h"
#include "shellundo_p.h"
#include "disks.h"
#include "mimeapps.h"
#include "desktopentry.h"

LSCommonFunctions *g_common_func = nullptr;

LSCommonFunctionsPrivate::LSCommonFunctionsPrivate(LSCommonFunctions *parent)
    : QObject()
    , d(parent)
    , m_undo(new QUndoStack)
    , m_opmgr(new OperationManager)
    , m_udisks(new LSUDisks)
    , m_mimeApps(new LSMimeApplications(this))
{
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &LSCommonFunctionsPrivate::clipboardDataChanged);

    m_mimeApps->cacheAllDesktops();
    // TODO: thread/optimize
    for(auto app : m_mimeApps->allApps())
    {
        if(app)
            app->checkForApk();
    }
}

QString LSCommonFunctionsPrivate::nextNewFolderName(const QUrl &path) const
{
    if(path.scheme() == "file")
    {
        auto local = path.toLocalFile();
        local.append("/");
        local.append(QApplication::tr("New Folder"));

        QDir dir(local);
        if(!dir.exists(local))
            return QApplication::tr("New Folder");

        uint curnum = 1;
        for(;;)
        {
            if(curnum > 1000)     // infinate loop safeguard
                return QString(); // clean up your FS ffs....

            auto local = path.toLocalFile();
            local.append("/");
            local.append(QApplication::tr("New Folder %1").arg(QString::number(curnum)));

            QDir dir(local);
            if(!dir.exists(local))
                return QApplication::tr("New Folder %1").arg(QString::number(curnum));

            curnum++;
        }
    }

    // TODO: libarchive schemes, etc
    return QString();
}

void LSCommonFunctionsPrivate::clipboardDataChanged()
{
    bool enable = false;
    uint count = 0;

    auto data = QApplication::clipboard()->mimeData();
    // see if it's something we can handle
    if(data->hasUrls())
    {
        for(auto u : data->urls())
        {
            if(u.scheme() == "file")
            {
                enable = true;
                break;
            }
        }
    }
    if(enable)
        count = data->urls().count();

    emit d->pasteAvailable(enable, count);
}

LSCommonFunctions::LSCommonFunctions()
    :p(new LSCommonFunctionsPrivate(this)) {}

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
    dlg->show();
}

QUrl LSCommonFunctions::newFolder(const QUrl &target, QWidget *parent)
{
    if(target.scheme() == "file")
    {
        // target should be a folder that we want to make a new folder inside of
        QDir dir(target.toLocalFile());
        if(!dir.exists(target.toLocalFile()))
        {
            showErrorDialog(tr("The folder %1 no longer exists.").arg(target.toLocalFile()),
                            tr("You can not create a new folder inside this location."),
                            parent);

            return QUrl();
        }

        auto newfolder = p->nextNewFolderName(target);
        auto fulllist = target.toLocalFile().split('/');
        fulllist.append(newfolder);
        auto fullfolder = fulllist.join('/');
        if(!fullfolder.isEmpty())
        {
            bool result = dir.mkdir(newfolder);
            if(!result)
            {
                showErrorDialog(tr("Failed to create folder %1.").arg(fullfolder),
                                tr("Check to make sure you have proper permissions, or contact your system administrator."),
                                parent);

                return QUrl();
            }
            // We have a new folder, so let's pop this onto the undo stack
            p->m_undo->beginMacro(tr("New Folder"));
            p->m_undo->push(new UndoNewFolder(target, newfolder));
            p->m_undo->endMacro();
            return newfolder;
        }
        else
        {
            showErrorDialog(tr("Failed to create folder %1.").arg(fullfolder),
                            tr("Could not find an elligable new folder name."),
                            parent);
        }
    }
    else
    {
        showErrorDialog(tr("Failed to create a new folder."),
                        tr("Could not create a folder inside %1: the scheme is unsupported.").arg(target.toString()),
                        parent);
    }
    return QString();
}

OperationManager *LSCommonFunctions::operationManager()
{
    return p->m_opmgr;
}

QUndoStack *LSCommonFunctions::undoStack()
{
    return p->m_undo;
}

LSUDisks *LSCommonFunctions::udiskManager()
{
    return p->m_udisks;
}

LSMimeApplications *LSCommonFunctions::mimeApps()
{
    return p->m_mimeApps;
}

void LSCommonFunctions::showErrorDialog(const QString &heading, const QString &msg, QWidget *parent, const QMessageBox::Icon icon)
{
    auto *dialog = new QMessageBox(parent);
    dialog->setModal(true);
    dialog->setIcon(icon);
    dialog->setWindowTitle(" ");
    dialog->setText(heading);
    dialog->setInformativeText(msg);
    dialog->setStandardButtons(QMessageBox::Ok);
    dialog->exec();

    delete dialog;
}

void LSCommonFunctions::questionAndStartUninstall(const QList<QUrl> desktopFiles)
{
#ifdef USE_APK
    if(desktopFiles.count() == 0)
        return;

    // TODO: set parent
    auto *dialog = new QMessageBox(0);
    if(desktopFiles.count() == 1)
    {
        auto file = desktopFiles.first().toLocalFile();
        QFileInfo fi(desktopFiles.first().toLocalFile());
        if(fi.isSymbolicLink())
            file = fi.symLinkTarget();

        auto desktop = mimeApps()->findDesktopForFile(file);
        if(desktop)
        {
            dialog->setWindowTitle(tr("Uninstall %1?").arg(desktop->value("Name").toString()));
            dialog->setText(tr("Do you want to uninstall %1?").arg(desktop->value("Name").toString()));
            dialog->setIcon(QMessageBox::Question);
            //dialog->setIconPixmap(desktop->icon().pixmap(QSize(48,48)));
            dialog->setInformativeText("This will remove program files, but your settings and documents will be preserved.");
        }
        else
        {
            dialog->deleteLater();
            return;
        }
    }
    else
    {
        dialog->setWindowTitle(tr("Uninstall %1 applications?").arg(QString::number(desktopFiles.count())));
        dialog->setText(tr("Do you want to uninstall the following applications?"));
        //dialog->setIconPixmap(QIcon::fromTheme("folder-trash").pixmap(QSize(64,64)));
        //dialog->setInformativeText(msg);
    }
    dialog->setModal(true);
    dialog->setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    int result = dialog->exec();
    delete dialog;
    if(result == QMessageBox::Yes)
    {

    }
#endif
}
