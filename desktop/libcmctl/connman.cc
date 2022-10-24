// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#include "private/connman_p.h"
#include "connman.h"
#include "agent.h"
#include "service.h"
#include "technology.h"
#include "model.h"
#include "wifimenu.h"
#include "config_dialog.h"

#include <QProcess>
#include <QDBusArgument>
#include <QMessageBox>

#include <syslog.h>
#include <QApplication>

namespace HWCM
{
ConnmanPrivate::ConnmanPrivate(ConnectionManager *p)
    : m_q(p)
    , m_agent(new ConnmanAgent(m_q))
    , m_model(new DeviceModel(this))
    , m_wifiModel(new WifiModel(this))
    , m_wifiMenu(new WifiMenu(this))
{
    // initialize our connman dbus agent
    con_manager = NULL;
    vpn_manager = NULL;
    if(!QDBusConnection::systemBus().isConnected())
        logErrors(Err_No_DBus);
    else
    {
        queryVersion();
        con_manager = new QDBusInterface(DBUS_CON_SERVICE, DBUS_PATH, DBUS_CON_MANAGER, QDBusConnection::systemBus(), this);
        if (!con_manager->isValid())
            logErrors(Err_Invalid_Con_Iface);
        else
        {
          // Access connman.manager to retrieve the data

          // Reset the getXX errors
          q16_errors &= ~Err_Properties;
          q16_errors &= ~Err_Technologies;
          q16_errors &= ~Err_Services;

          if (!getTechnologies())
              logErrors(Err_Technologies);

          if (!getServices())
              logErrors(Err_Services);

          if (!getProperties())
              logErrors(Err_Properties);

          // register the agent
          processReply(con_manager->call(QDBus::AutoDetect, "RegisterAgent", QVariant::fromValue(QDBusObjectPath(AGENT_OBJECT))));

          // if counters are enabled connect signal to slot and register the counter
          /*
             QDBusMessage reply = con_manager->callWithArgumentList(QDBus::AutoDetect, "RegisterCounter", vlist_counter);
             if (processReply(reply) == QDBusMessage::ReplyMessage)
                connect(counter, SIGNAL(usageUpdated(QDBusObjectPath, QString, QString)), this, SLOT(counterUpdated(QDBusObjectPath, QString, QString)));
          } */

        // connect some dbus signals to our slots
        QDBusConnection::systemBus().connect(DBUS_CON_SERVICE, DBUS_PATH, DBUS_CON_MANAGER, "PropertyChanged", this, SLOT(dbsPropertyChanged(QString, QDBusVariant)));
        QDBusConnection::systemBus().connect(DBUS_CON_SERVICE, DBUS_PATH, DBUS_CON_MANAGER, "ServicesChanged", this, SLOT(dbsServicesChanged(QList<QVariant>, QList<QDBusObjectPath>, QDBusMessage)));
        QDBusConnection::systemBus().connect(DBUS_CON_SERVICE, DBUS_PATH, DBUS_CON_MANAGER, "PeersChanged", this, SLOT(dbsPeersChanged(QList<QVariant>, QList<QDBusObjectPath>, QDBusMessage)));
        QDBusConnection::systemBus().connect(DBUS_CON_SERVICE, DBUS_PATH, DBUS_CON_MANAGER, "TechnologyAdded", this, SLOT(dbsTechnologyAdded(QDBusObjectPath, QVariantMap)));
        QDBusConnection::systemBus().connect(DBUS_CON_SERVICE, DBUS_PATH, DBUS_CON_MANAGER, "TechnologyRemoved", this, SLOT(dbsTechnologyRemoved(QDBusObjectPath)));

        // clear the counters if selected
        //clearCounters();
        m_model->rescanSys();
       }
    }
}

ConnmanPrivate::~ConnmanPrivate()
{
    if(con_manager->isValid())
    {
       // agent
       processReply(con_manager->call(QDBus::AutoDetect, "UnregisterAgent",
                QVariant::fromValue(QDBusObjectPath(AGENT_OBJECT))) );
       // counter - only have a signal-slot connection if the counter was able to be registered
       /*if(counter->cnxns() > 0)
            processReply(con_manager->call(QDBus::AutoDetect, "UnregisterCounter", QVariant::fromValue(QDBusObjectPath(CNTR_OBJECT))) );
        */

      /* if (vpn_manager != NULL) {
          if (vpn_manager->isValid() ) {
             shared::processReply(vpn_manager->call(QDBus::AutoDetect, "UnregisterAgent", QVariant::fromValue(QDBusObjectPath(VPN_AGENT_OBJECT))) );
          } // ivpn_manager isValid
       } // not null*/

    }
}

void ConnmanPrivate::logErrors(const quint16 &err)
{
    // store the error in a data element
    q16_errors |= err;

    // Log the error in the system log
    // LOG_PID Include PID with each message
    // LOG_CONS Write to console if there is a problem writing to system log
    // LOG_USER User Level Message
    // LOG_ERR Error condition
    // LOG_WARNING    Warning contition
    // Defined in resource.h
    // LOG_NAME Name to display in the log

    // TODO: define a parent widget/window in the class?
    auto parent = nullptr;
    openlog(qPrintable(LOG_NAME), LOG_PID|LOG_CONS, LOG_USER);
    switch(err)
    {
       case  Err_No_DBus:
          syslog(LOG_ERR, "%s", tr("Could not find a connection to the system bus").toUtf8().constData() );
          QMessageBox::critical(parent, tr("%1 - Critical Error").arg(QLatin1String("cmst")),
             tr("Unable to find a connection to the system bus.<br><br>%1 will not be able to communicate with connman.").arg(QLatin1String("cmst")) );
          break;
       case  Err_Invalid_Con_Iface:
          syslog(LOG_ERR, "%s",tr("Could not create an interface to connman on the system bus").toUtf8().constData());
          QMessageBox::critical(parent, tr("%1 - Critical Error").arg(QLatin1String("cmst")),
             tr("Unable to create an interface to connman on the system bus.<br><br>%1 will not be able to communicate with connman.").arg(QLatin1String("cmst")) );
          break;
       case  Err_Properties:
          syslog(LOG_ERR, "%s", tr("Error reading or parsing connman.Manager.GetProperties").toUtf8().constData() );
          QMessageBox::warning(parent, tr("%1 - Warning").arg(QLatin1String("cmst")),
             tr("There was an error reading or parsing the reply from method connman.Manager.GetProperties.<br><br>It is unlikely any portion of %1 will be functional.").arg(QLatin1String("cmst")) );
          break;
       case  Err_Technologies:
          syslog(LOG_ERR, "%s",tr("Error reading or parsing connman.Manager.GetTechnologies").toUtf8().constData() );
          QMessageBox::warning(parent, tr("%1 - Warning").arg(QLatin1String("cmst")),
             tr("There was an error reading or parsing the reply from method connman.Manager.GetTechnologies.<br><br>Some portion of %1 may still be functional.").arg(QLatin1String("cmst")) );
          break;
       case  Err_Services:
          syslog(LOG_ERR, "%s", tr("Error reading or parsing connman.Manager.GetServices").toUtf8().constData() );
          QMessageBox::warning(parent, tr("%1 - Warning").arg(QLatin1String("cmst")),
             tr("There was an error reading or parsing the reply from method connman.Manager.GetServices.<br><br>Some portion of %1 may still be functional.").arg(QLatin1String("cmst")) );
          break;
       case  Err_Invalid_VPN_Iface:
          // NOTE: this error is logged to the system log only, no message is presented to the user (issue #155).  Done
          // this way because it will happen when someone does not compile ConnMan with VPN support, or if they don't
          // have the connman-vpn daemon running.  We're figuring this would be more than likely intentional and not an
          // error the user would need to be concerned about.  If he is the error will show in the system log.
          syslog(LOG_ERR, "%s",tr("Could not create an interface to connman-vpn on the system bus").toUtf8().constData());
          break;
       default:
          break;
       }
  closelog();

  return;
}

bool ConnmanPrivate::getProperties()
{
    // call connman and GetProperties
    QDBusMessage reply = con_manager->call("GetProperties");
    processReply(reply);

    // call the function to get the map values
    properties_map.clear();
    auto val = getMap(properties_map, reply);
    return val;
}

bool ConnmanPrivate::getTechnologies()
{
    // call connman and GetTechnologies
    QDBusMessage reply = con_manager->call("GetTechnologies");
    processReply(reply);

    // call the function to get the map values
    technologies_list.clear();
    auto ga_result = getArray(technologies_list, reply);
    if(ga_result)
    {
        // map our values to classes
        for(auto d : technologies_list)
        {
            auto tech = new Technology(m_q, d);
            m_technologies.append(tech);
        }
    }
    // this is a temp list - clear it
    technologies_list.clear();
    return ga_result;
}

bool ConnmanPrivate::getServices()
{
    // call connman and GetServices
    QDBusMessage reply = con_manager->call("GetServices");
    processReply(reply);

    // call the function to get the map values
    services_list.clear();
    bool ga_result = getArray(services_list, reply);
    if(ga_result)
    {
        // map our values to classes
        for(auto d : services_list)
        {
            auto svc = new Service(m_q, d);
            m_services.append(svc);
            if(svc->type() == "wifi")
                m_wifiNetworks.append(svc);
        }
    }
    // this is a temp list - clear it
    services_list.clear();
    return ga_result;
}

bool ConnmanPrivate::getArray(QList<arrayElement> &r_list, const QDBusMessage &r_msg)
{
    // make sure r_msg is a QDBusArgument
    if (!r_msg.arguments().at(0).canConvert<QDBusArgument>())
        return false;

    // make sure the QDBusArgument holds an array
    const QDBusArgument &qdb_arg = r_msg.arguments().at(0).value<QDBusArgument>();
    if (qdb_arg.currentType() != QDBusArgument::ArrayType)
        return false;

    // iterate over the QDBusArgument pulling array elements out and inserting into
    // an arrayElement structure.
    qdb_arg.beginArray();
    r_list.clear();

    while(!qdb_arg.atEnd())
    {
       // make sure the argument is a structure type
       if(qdb_arg.currentType() != QDBusArgument::StructureType)
           return false;

       arrayElement ael;
       qdb_arg.beginStructure();
       qdb_arg >> ael.objpath >> ael.objmap;
       qDebug() << ael.objpath.path() << ael.objmap;

       qdb_arg.endStructure();
       r_list.append (ael);
    }

    qdb_arg.endArray();

    return true;
}

bool ConnmanPrivate::getMap(QMap<QString, QVariant> &r_map, const QDBusMessage &r_msg)
{
    // make sure r_msg is a QDBusArgument
    if (!r_msg.arguments().at(0).canConvert<QDBusArgument>())
        return false;

    // make sure the QDBusArgument holds a map
    const QDBusArgument &qdb_arg = r_msg.arguments().at(0).value<QDBusArgument>();
    if (qdb_arg.currentType() != QDBusArgument::MapType)
        return false;

    // iterate over the QDBusArgument pulling map keys and values out
    qdb_arg.beginMap();
    r_map.clear();

    while (!qdb_arg.atEnd())
    {
       QString key;
       QVariant value;
       qdb_arg.beginMapEntry();

       qdb_arg.endMapEntry();
       r_map.insert(key, value);
    }
    qdb_arg.endMap();

    return true;
}

QDBusMessage::MessageType ConnmanPrivate::processReply(const QDBusMessage &reply)
{
    if (reply.type() != QDBusMessage::ReplyMessage) {
      QMessageBox::warning(0,
          QString(tr("cmst") + qApp->translate("processReply", " Warning") ),
          qApp->translate("processReply",
            "<center><b>We received a DBUS reply message indicating an error.</b></center>"
            "<br><br>Error Name: %1<br><br>Error Message: %2")
              .arg(reply.errorName())
              .arg(reply.errorMessage()));
     } // if reply is something other than a normal reply message

    return reply.type();
}

bool ConnmanPrivate::extractMapData(QMap<QString,QVariant>& r_map, const QVariant& r_var)
{
    //  make sure we can convert the QVariant into a QDBusArgument
    if (!r_var.canConvert<QDBusArgument>())
        return false;

    const QDBusArgument qdba =
            r_var.value<QDBusArgument>();

    // make sure the QDBusArgument holds a map
    if (qdba.currentType() != QDBusArgument::MapType)
        return false;

    // iterate over the QDBusArgument pulling map keys and values out
      r_map.clear();
      qdba.beginMap();

      while (!qdba.atEnd())
      {
        QString key;
        QVariant value;
        qdba.beginMapEntry();
        qdba >> key >> value;
        qdba.endMapEntry();
        r_map.insert(key, value);
      }

      qdba.endMap();
      return true;
}

void ConnmanPrivate::queryVersion()
{
    QProcess qps;
    bool b_ok = false;

    qps.start("connmand", {"-v"});
    qps.waitForFinished();
    f_connmanversion = qps.readAllStandardOutput().toFloat(&b_ok);
    if (! b_ok) f_connmanversion = -1.0;
    return;
}

void ConnmanPrivate::dbusPropertyChanged(QString prop, QDBusVariant dbvalue)
{
    // save current state and update propertiesMap
    QString oldstate = properties_map.value(prop).toString();
    properties_map.insert(prop, dbvalue.variant() );

    // offlinemode property
    if (prop == "OfflineMode")
    {
    /*   notifyclient->init();
        // TODO notifications
       this->sendNotifications();*/
    } // if contains offlinemode


    // state property
    if (prop == "State")
    {
       /* // local variables
       QString state = dbvalue.variant().toString();

       // send notification if state is not ready or online
       notifyclient->init();
       notifyclient->setSummary(tr("Network Services:") );
       if (state == "ready" || state == "online") {
          if (oldstate != "ready" && oldstate != "online") {
             notifyclient->setBody(tr("The system is online.") );
             notifyclient->setIcon(iconman->getIconName("state_online") );
             this->sendNotifications();
          } // if
       } // if
       else {
          notifyclient->setBody(tr("The system is offline.") );
          notifyclient->setIcon(iconman->getIconName("state_not_ready") );
          this->sendNotifications();
       } // else */

       // execute external program if specified
       /* if (! ui.lineEdit_afterconnect->text().isEmpty()   ) {
          if( (state == "ready" || state == "online") && (oldstate != "ready" && oldstate != "online") ) {
             QString text = ui.lineEdit_afterconnect->text();
             text = text.simplified();
             QStringList args = text.split(' ');
             QString cmd = args.first();
             args.removeFirst();
             QProcess* proc = new QProcess(this);
             proc->startDetached(cmd, args);
          } // if online or ready and not online before
       } // if lineedit not empty */

    } // if state change
}

void ConnmanPrivate::dbusServicesChanged(QList<QVariant>, QList<QDBusObjectPath>, QDBusMessage)
{
    m_model->rescanSys();
}

void ConnmanPrivate::dbusPeersChanged(QList<QVariant>, QList<QDBusObjectPath>, QDBusMessage)
{

}

void ConnmanPrivate::dbusTechnologyAdded(QDBusObjectPath, QVariantMap)
{

}

void ConnmanPrivate::dbusTechnologyRemoved(QDBusObjectPath)
{

}

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_d(new ConnmanPrivate(this))
{}

ConnectionManager::~ConnectionManager()
{
    delete m_d;
}

DeviceModel* ConnectionManager::model() const
{
    return const_cast<DeviceModel*>(m_d->m_model);
}

WifiModel *ConnectionManager::wifiModel() const
{
    return const_cast<WifiModel*>(m_d->m_wifiModel);
}

WifiMenu *ConnectionManager::wifiMenu() const
{
    return const_cast<WifiMenu*>(m_d->m_wifiMenu);
}

Service *ConnectionManager::serviceForName(QString svcName) const
{
    for(auto s : m_d->m_services)
    {
        if(s->nickName() == svcName)
            return s;
    }

    return nullptr;
}

ConfigDialog *ConnectionManager::getConfigDialog(Service *svc, QWidget *parent)
{
    if(svc == nullptr)
        return nullptr;

    auto cfg = new ConfigDialog(svc, parent);

    return cfg;
}

QList<Service *> ConnectionManager::services()
{
    return m_d->m_services;
}


} // END namespace HWCM
