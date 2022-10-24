#include "openrc.h"

#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QVector>
#include <QSettings>
#include "openrcmodel.h"

OpenRCProcess::OpenRCProcess(const QString &proc, QObject *parent)
    : QObject(parent)
    , m_proc(new QProcess(this))
    , m_name(proc)
{
    connect(m_proc, &QProcess::finished, this, &OpenRCProcess::readProcData);
    m_proc->setProgram(OPENRC_SERVICE_BINARY);
    m_proc->setArguments(QStringList() << "-r" << proc);
    m_proc->start(QProcess::ReadOnly);
}

void OpenRCProcess::setStarted(const bool st)
{
    m_started = st;
}

void OpenRCProcess::addRunlevel(const QString &rl)
{
    m_runlevels.append(rl);
}

void OpenRCProcess::readProcData()
{
    m_initpath = m_proc->readAllStandardOutput().trimmed();
    qDebug() << m_initpath;
    m_proc->deleteLater();

    QFile file(m_initpath);
    if(file.open(QFile::ReadOnly))
    {
        auto data = file.readAll().split('\n');
        file.close();

        bool has_desc = false;
        bool has_ec = false;
        bool has_ecs = false;

        QList<QByteArray> cmds;
        QList<QByteArray> cmds_started;

        for(auto line : data)
        {
            if(line.startsWith("description=") && !has_desc)
            {
                auto desc = line.split('=');
                desc.removeFirst();
                auto line = desc.join('=');
                line = line.remove(0, 1); // remove first "
                line = line.remove(line.length()-1, 1); // remove last "
                m_description = line;
                has_desc = true;
            }
            if(line.startsWith("extra_commands="))
            {
                auto desc = line.split('=');
                desc.removeFirst();
                auto line = desc.join('=');
                line = line.remove(0, 1); // remove first "
                line = line.remove(line.length()-1, 1); // remove last "

                cmds = line.split(' ');
                has_ec = true;
            }
            if(line.startsWith("extra_started_commands="))
            {
                auto desc = line.split('=');
                desc.removeFirst();
                auto line = desc.join('=');
                line = line.remove(0, 1); // remove first "
                line = line.remove(line.length()-1, 1); // remove last "

                cmds_started = line.split(' ');
                has_ecs = true;
            }
        }
    }
}

QString OpenRCProcess::status() const
{
    if(m_started)
        return tr("Started");
    else
        return tr("Stopped");
}

QString OpenRCProcess::name() const
{
    return m_name;
}

QString OpenRCProcess::description() const
{
    return m_description;
}

OpenRCMonitor::OpenRCMonitor(QObject *parent)
    : QObject(parent),
      m_statmon(new QProcess(this))
{
    m_statmon->setProgram(OPENRC_STATUS_BINARY);
    // openrc-status -ivf ini # gets an INI style output of
    // openrc service status
    m_statmon->setArguments(QStringList() << "-avf" << "ini");
    connect(m_statmon, &QProcess::finished, this, &OpenRCMonitor::processFinished);
    refreshStatus();
    m_model = new OpenRCServiceModel(this);
}

bool OpenRCMonitor::systemHasOpenRCInit()
{
    QFile file(OPENRC_STATUS_BINARY);
    if(!file.exists())
        return false;

    return true;
}

OpenRCServiceModel *OpenRCMonitor::model()
{
    return m_model;
}

void OpenRCMonitor::refreshStatus()
{
    m_statmon->start();
}

void OpenRCMonitor::processFinished()
{
    auto dt = m_statmon->readAll().split('\n');
    QByteArray currentGroup = "";

   for(int i = 0; i < dt.count(); i++)
   {
       auto line = dt[i];
       if(line.startsWith('['))
           currentGroup = line.remove(0,1).remove(line.length()-1,1);
       else if(line.contains('='))
       {
            auto key = line.split('=').first().trimmed();
            auto valList = line.split('=');
            valList.removeFirst();
            auto val = valList.join('=').trimmed();
            if(!m_processlist.contains(key))
            {
                auto *p = new OpenRCProcess(key, this);
                p->addRunlevel(currentGroup);
                m_processlist.insert(key,p);
                if(val == QByteArray("stopped"))
                    p->setStarted(false);
                else
                    p->setStarted(true);
            }
            else
                m_processlist.value(key)->addRunlevel(currentGroup);
       }
   }
   m_model->update();
}
