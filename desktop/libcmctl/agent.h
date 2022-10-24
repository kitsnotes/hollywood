// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef AGENT_H
#define AGENT_H

#include "libcmctl_global.h"

#define AGENT_SERVICE "org.originull"
#define AGENT_INTERFACE "net.connman.Agent"
#define AGENT_OBJECT "/org/originull/NetworkAgent"

namespace HWCM
{
    class ConnmanAgentPrivate;
    class LIBCMCTL_EXPORT ConnmanAgent : public QObject, protected QDBusContext
    {
       Q_OBJECT
       Q_CLASSINFO("D-Bus Interface", AGENT_INTERFACE)
       public:
          ConnmanAgent(QObject*);
          void setLogInputRequest(bool b);
       public Q_SLOTS:
          void Release();
          void ReportError(QDBusObjectPath, QString);
          void RequestBrowser(QDBusObjectPath, QString);
          QVariantMap RequestInput(QDBusObjectPath, QMap<QString,QVariant>);
          void Cancel();
       private:
          ConnmanAgentPrivate *m_d = nullptr;
    };
}
#endif
