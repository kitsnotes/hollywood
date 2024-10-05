// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

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
