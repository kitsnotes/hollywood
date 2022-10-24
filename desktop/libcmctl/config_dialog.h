// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <QDialog>
#include <QString>
#include <QVariant>

namespace HWCM
{
    class Service;
    class ConfigDialogPrivate;
    class ConfigDialog : public QDialog
    {
    Q_OBJECT
    public:
      explicit ConfigDialog(Service *svc, QWidget *parent = nullptr);
    private:
        friend class ConfigDialogPrivate;
        ConfigDialogPrivate* m_p;
    };
}

#endif // CONFIG_DIALOG_H
