// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#ifndef VIEWOPTIONSDIALOG_P_H
#define VIEWOPTIONSDIALOG_P_H

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

#endif // VIEWOPTIONSDIALOG_P_H
