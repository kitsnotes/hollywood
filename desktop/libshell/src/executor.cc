#include "executor.h"
#include "executor_p.h"

#include "desktopentry.h"
#include "directories.h"
#include "mimeapps.h"

#include <QMimeType>
#include <QMimeData>
#include <QMimeDatabase>

ExecutorPrivate::ExecutorPrivate(LSExecutor *parent)
    : d(parent) {}

ExecutorPrivate::~ExecutorPrivate()
{
    for(auto proc : m_processList)
        proc->deleteLater();
    delete m_dt;
}

bool ExecutorPrivate::findBestApplication()
{
    if(m_dt != nullptr)
        return false;

    if(m_files.count() == 0)
        return false;

    // find the mime type by file
    auto file = m_files.first();
    if(!QFile::exists(file))
        return false;

    QMimeDatabase md;
    auto mime = md.mimeTypeForFile(file);
    if(mime.isValid())
    {
        auto ma = new LSMimeApplications(d);

        m_dt = ma->defaultApp(mime.name());
        ma->deleteLater();
        return true;
    }

    return false;
}

bool ExecutorPrivate::createProcess(QString &exec, QStringList args)
{
    auto proc = new QProcess(d);
    proc->setProgram(exec);
    proc->setArguments(args);
    auto env = QProcessEnvironment::systemEnvironment();
    env.remove("HW_TWILIGHT_SHELL");
    proc->setProcessEnvironment(env);

    bool start = proc->startDetached();
    if(start)
    {
        m_processList.append(proc);
        QObject::connect(proc, &QProcess::finished, d, [proc, this]() {
           m_processList.removeOne(proc);
           proc->deleteLater();
           emit d->terminated(proc->exitCode());
           if(m_processList.isEmpty())
               delete d;
        });
        return true;
    }
    else
    {
        proc->deleteLater();
        return false;
    }
}

LSExecutor::LSExecutor(QObject *parent)
    : QObject(parent)
    , p(new ExecutorPrivate(this)) {}

LSExecutor::~LSExecutor()
{
    delete p;
}

bool LSExecutor::setDesktopFile(const QString &desktop)
{
    qDebug() << desktop;
    auto dt =
            LSDesktopEntry::findDesktopFile(desktop);
    if(dt.isEmpty())
        return false;

    auto dtf = new LSDesktopEntry();
    dtf->load(dt);

    p->m_dt = dtf;
    return true;
}


void LSExecutor::setOpenFiles(const QStringList &files)
{
    p->m_files = files;
}

void LSExecutor::setArguments(const QStringList &arguments)
{
    p->m_arguments = arguments;
}

void LSExecutor::addArgument(const QString &argument)
{
    p->m_arguments.append(argument);
}

bool LSExecutor::launch()
{
    if(p->m_dt == nullptr)
        return false;

    auto exec = p->m_dt->value("Exec").toString();

    qDebug() << exec;
    // deprecated params according to freedesktop
    exec.replace("%d", "");
    exec.replace("%D", "");
    exec.replace("%n", "");
    exec.replace("%N", "");
    exec.replace("%v", "");
    exec.replace("%m", "");

    if(exec.contains("%i"))
    {
        auto icon = p->m_dt->value("Icon").toString();
        exec.replace("%i", QString("--icon %1").arg(icon));
    }

    if(exec.contains("%c"))
    {
        auto lc = p->m_dt->localizedValue("Name").toString();
        exec.replace("%c", lc);
    }

    if(exec.contains("%k"))
        exec.replace("%k", p->m_dt->fileName());

    // replace our files and execute
    if(exec.contains("%u") || exec.contains("%U"))
    {
        qDebug() << "starting url";

        // we support URL's
        if(exec.contains("%U"))
        {
            // handle our arguments
            if(!p->m_arguments.isEmpty())
                exec.replace("%U", QString("%1 %U").arg(p->m_arguments.join(' ')));

            exec.replace("%f", "");
            exec.replace("%F", "");
            exec.replace("%u", "");
            // our preference is for multiple lists
            QStringList urlList;
            for(auto f : p->m_files)
            {
                QUrl u(f);
                urlList.append(u.toString(QUrl::None));
            }
            exec.replace("%U", urlList.join(' '));

            // launch our process
            auto args = exec.split(' ');
            auto execBin = args.takeFirst();

            return p->createProcess(execBin, args);
        }
        else
        {
            // we only have ability to launch 1
            // handle our arguments
            if(!p->m_arguments.isEmpty())
                exec.replace("%u", QString("%1 %u").arg(p->m_arguments.join(' ')));

            exec.replace("%U", "");
            exec.replace("%f", "");
            exec.replace("%F", "");

            bool one_launched = false;
            for(auto fn : p->m_files)
            {
                QUrl u(fn);
                auto execCopy = exec;
                execCopy.replace("%u", u.toString(QUrl::None));
                auto args = execCopy.split(' ');
                auto execBin = args.takeFirst();
                bool ret = p->createProcess(execBin, args);

                if(ret)
                    one_launched = ret;
            }
            return one_launched;
        }
    }
    else if(exec.contains("%f") || exec.contains("%F"))
    {
        qDebug() << "starting file";
        // we support just files
        if(exec.contains("%F"))
        {
            QStringList s;
            for(QString fn : p->m_files)
                s.append(QString("\"%1\"").arg(fn));

            exec.replace("%F", s.join(' '));
            // replace extraneous things
            exec.replace("%u", "");
            exec.replace("%U", "");
            exec.replace("%f", "");
            // launch our process
            auto args = exec.split(' ');
            auto execBin = args.takeFirst();
            qDebug() << execBin << args;

            return p->createProcess(execBin, args);
        }
        else
        {
            // we only have ability to launch 1
            // handle our arguments
            if(!p->m_arguments.isEmpty())
                exec.replace("%f", QString("%1 %f").arg(p->m_arguments.join(' ')));

            exec.replace("%U", "");
            exec.replace("%u", "");
            exec.replace("%F", "");

            // loop for launching processes
            bool one_launched = false;
            for(auto fn : p->m_files)
            {
                auto execCopy = exec;
                execCopy.replace("%u", QString("\"%1\"").arg(fn));
                auto args = execCopy.split(' ');
                auto execBin = args.takeFirst();
                bool ret = p->createProcess(execBin, args);
                qDebug() << execBin << args;

                if(ret)
                    one_launched = ret;
            }
            return one_launched;
        }
    }
    else
    {
        qDebug() << "starting null";
        // we have no %uU or %fF so um...
        // i guess we can try and launch the thing with a single param as file
        if(!p->m_arguments.isEmpty())
            exec.append(QString(" %1").arg(p->m_arguments.join(' ')));

        // loop for launching processes
        if(p->m_files.count() > 0)
        {
            bool one_launched = false;
            for(auto fn : p->m_files)
            {
                auto execCopy = exec;
                execCopy.append(QString(" \"%1\"").arg(fn));
                auto args = execCopy.split(' ');
                auto execBin = args.takeFirst();
                bool ret = p->createProcess(execBin, args);

                if(ret)
                    one_launched = ret;
            }
            return one_launched;
        }
        else
        {
            auto execCopy = exec;
            auto args = execCopy.split(' ');
            auto execBin = args.takeFirst();
            return p->createProcess(execBin, args);
        }
    }

    return false;
}

void LSExecutor::terminate()
{
    for(auto p : p->m_processList)
        p->terminate();
}

void LSExecutor::kill()
{
    for(auto p : p->m_processList)
        p->kill();
}

uint LSExecutor::runningProcesses() const
{
    uint run = 0;
    for(auto p : p->m_processList)
    {
        if(p->state() == QProcess::Running)
            run++;
    }

    return run;
}
