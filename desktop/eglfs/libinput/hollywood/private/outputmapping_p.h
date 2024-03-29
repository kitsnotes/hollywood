// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef QOUTPUTMAPPING_P_H
#define QOUTPUTMAPPING_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QString>
#include <QHash>
#include <private/qglobal_p.h>

class QWindow;

namespace Originull {

namespace Platform {

class QOutputMapping
{
public:
    virtual ~QOutputMapping() {}

    static QOutputMapping *get();
    virtual bool load();
    virtual QString screenNameForDeviceNode(const QString &deviceNode);

#ifdef Q_OS_WEBOS
    virtual QWindow *windowForDeviceNode(const QString &deviceNode);
    static void set(QOutputMapping *mapping);
#endif
};

class QDefaultOutputMapping : public QOutputMapping
{
public:
    bool load() override;
    QString screenNameForDeviceNode(const QString &deviceNode) override;

private:
    QHash<QString, QString> m_screenTable;
};

}
}
#endif // QOUTPUTMAPPING_P_H
