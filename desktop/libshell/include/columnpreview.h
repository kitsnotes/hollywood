// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#ifndef LSCOLUMNPREVIEWWIDGET_H
#define LSCOLUMNPREVIEWWIDGET_H

#include <QWidget>

#include "libshell_int.h"

class LSColumnPreviewPrivate;
class LSColumnPreview : public QWidget
{
    Q_OBJECT
public:
    explicit LSColumnPreview(QWidget *parent = nullptr);
    ~LSColumnPreview();
    void updateFileInfo(QFileInfo info);
signals:
private:
    LSColumnPreviewPrivate *p;
};

#endif // LSCOLUMNPREVIEWWIDGET_H
