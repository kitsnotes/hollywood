// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef LIBCMCTL_GLOBAL_H
#define LIBCMCTL_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QVariantMap>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusContext>

// Program Values:
//  QApplication (not user visible)
//  QSettings (visible in filesystem only)
//  System Logging (visible in system logs only)
#define LONG_NAME "Hollywood Network Settings"
#define ORG "originull"
#define APP "hwnetwork"
#define LOG_NAME "HOLLYWOODNET"
#define IPT_REQ_LOG_PATH "/tmp/hwcm"
#define IPT_REQ_LOG_FILE "input_request.log"

#define DBUS_PATH "/"
#define DBUS_CON_SERVICE "net.connman"
#define DBUS_VPN_SERVICE "net.connman.vpn"
#define DBUS_CON_MANAGER "net.connman.Manager"
#define DBUS_VPN_MANAGER "net.connman.vpn.Manager"


#if defined(LIBCMCTL_LIBRARY)
#  define LIBCMCTL_EXPORT Q_DECL_EXPORT
#else
#  define LIBCMCTL_EXPORT Q_DECL_IMPORT
#endif

//  Used for enum's local to this program

namespace HWCM
{
  enum {
    // errors
    No_Errors             = 0x00,
    Err_No_DBus           = (1 << 0), // Can't find DBus
    Err_Invalid_Con_Iface = (1 << 1), // Invalid interface
    Err_Properties        = (1 << 2), // There was an error reading connman.Manager.GetProperties
    Err_Technologies      = (1 << 3), // There was an error reading connman.Manager.GetTechnologies
    Err_Services          = (1 << 4), // There was an error reading connman.Manager.GetServices
    Err_Invalid_VPN_Iface = (1 << 5), // Invalid interface

    // provisioning editor
    ProvEd_No_Selection = 0x00,
    ProvEd_File_Read    = (1 << 0),
    ProvEd_File_Delete  = (1 << 1),
    ProvEd_File_Write   = (1 << 2),

    // validating dialog validator input
    ValDialog_None      = 0x00,
    ValDialog_IPv4      = 0x01,
    ValDialog_nmask4    = 0x02,
    ValDialog_IPv6      = 0x03,
    ValDialog_MAC       = 0x04,
    ValDialog_46        = 0x05,
    ValDialog_Hex       = 0x06,
    ValDialog_Int       = 0x07,
    ValDialog_Dom       = 0x08,
    ValDialog_Word      = 0x09,
    ValDialog_min1ch    = 0x0a,
    ValDialog_min8ch    = 0x0b,
//  ValDialog_46d       = 0x0c,
    ValDialog_IPv4cidr  = 0x0d,
    ValDialog_IPv6cidr  = 0x0e,
    ValDialog_46cidr    = 0x0f,
    ValDialog_networks  = 0x10,
  };  // enum

  struct arrayElement
  {
     QDBusObjectPath objpath;
     QMap<QString,QVariant> objmap;
  };
} // namespace HWCM


#endif // LIBCMCTL_GLOBAL_H
