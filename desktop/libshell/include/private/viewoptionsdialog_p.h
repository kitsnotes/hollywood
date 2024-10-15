// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QVBoxLayout>
#include "libshell_int.h"

class LSIconViewOptions;
class LSViewOptionsDialog;
class LSViewOptionsDialogPrivate
{
private:
    friend class LSViewOptionsDialog;
    LSViewOptionsDialogPrivate(LSViewOptionsDialog *parent);
    ~LSViewOptionsDialogPrivate() = default;
    LSViewOptionsDialog *d;
    QVBoxLayout *m_layout;
    HWShell::ViewMode m_viewMode;
    LSIconViewOptions *m_icons;

    QWidget *m_current = nullptr;
};

