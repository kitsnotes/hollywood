// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <hollywood/preferencedialog.h>
#include "libshell_int.h"

class LSGetInfoDialogPrivate;
class LIBSHELL_EXPORT LSGetInfoDialog : public HWPreferenceDialog
{
    Q_OBJECT
public:
    LSGetInfoDialog(const QUrl &file, QWidget *parent = nullptr);
    void reloadInfo();
public slots:
    void open();
private:
    LSGetInfoDialogPrivate *p;
};

