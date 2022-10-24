// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef AGENT_P_H
#define AGENT_P_H
#include <QObject>
#include <QMap>

namespace HWCM
{
    class WifiDialog;
    class ConnmanAgent;
    class ConnmanAgentPrivate : public QObject
    {
       Q_OBJECT
       public:
          ConnmanAgentPrivate(ConnmanAgent *q);

          inline void setLogInputRequest(bool b) {b_loginputrequest = b;}
       private:
          friend class ConnmanAgent;
          void createInputMap(const QMap<QString,QVariant>&);
          ConnmanAgent *m_q;
          WifiDialog* m_wifi;
          QMap<QString,QString> input_map;
          bool b_loginputrequest;
    };
}
#endif // AGENT_P_H
