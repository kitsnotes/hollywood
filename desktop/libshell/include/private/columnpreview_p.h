// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class LSColumnPreview;
class LSColumnPreviewPrivate
{
private:
    friend class LSColumnPreview;
    LSColumnPreviewPrivate(LSColumnPreview *parent);
    ~LSColumnPreviewPrivate() = default;
    LSColumnPreview *d;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_previewLayout;
    QSpacerItem *m_previewSpacerLeft;
    QGraphicsView *m_preview;
    QSpacerItem *m_previewSpacerRight;
    QSpacerItem *m_underPreviewSpacer;
    QLabel *m_fileTitle;
    QHBoxLayout *m_fileSubTitleLayout;
    QSpacerItem *m_fileSubTitleSpacer;
    QLabel *m_fileSubtitle;
    QTableWidget *m_fileProperties;
    QSpacerItem *m_endSpacer;
};
