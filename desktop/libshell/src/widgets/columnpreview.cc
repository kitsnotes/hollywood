// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "columnpreview_p.h"
#include "columnpreview.h"

LSColumnPreviewPrivate::LSColumnPreviewPrivate(LSColumnPreview *parent)
    : d(parent)
{
    m_mainLayout = new QVBoxLayout(parent);
    m_previewLayout = new QHBoxLayout();
    m_previewSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_previewSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_underPreviewSpacer = new QSpacerItem(20, 15, QSizePolicy::Minimum, QSizePolicy::Maximum);
    m_fileSubTitleLayout = new QHBoxLayout();
    m_fileSubTitleSpacer = new QSpacerItem(10, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_preview = new QGraphicsView(parent);
    m_fileTitle = new QLabel(parent);
    m_fileSubtitle = new QLabel(parent);
    m_fileProperties = new QTableWidget(parent);
    m_endSpacer = new QSpacerItem(20, 132, QSizePolicy::Minimum, QSizePolicy::Expanding);

    m_mainLayout->setObjectName(QString::fromUtf8("MainLayout"));
    m_previewLayout->setObjectName(QString::fromUtf8("PreviewLayout"));
    m_preview->setObjectName(QString::fromUtf8("PreviewWidget"));
    m_fileTitle->setObjectName(QString::fromUtf8("FileTitle"));
    m_fileSubTitleLayout->setObjectName(QString::fromUtf8("FileSubTitleLayout"));
    m_fileSubtitle->setObjectName(QString::fromUtf8("FileSubTitle"));

    m_fileProperties->setFrameStyle(QFrame::NoFrame);
    m_preview->setFrameStyle(QFrame::NoFrame);

    m_previewLayout->addItem(m_previewSpacerLeft);
    m_previewLayout->addWidget(m_preview);
    m_previewLayout->addItem(m_previewSpacerRight);

    m_fileSubTitleLayout->addItem(m_fileSubTitleSpacer);
    m_fileSubTitleLayout->addWidget(m_fileSubtitle);

    m_mainLayout->addLayout(m_previewLayout);
    m_mainLayout->addItem(m_underPreviewSpacer);
    m_mainLayout->addWidget(m_fileTitle);
    m_mainLayout->addLayout(m_fileSubTitleLayout);
    m_mainLayout->addWidget(m_fileProperties);
    m_mainLayout->addItem(m_endSpacer);
    m_fileTitle->setText("FileTitle");
    m_fileSubtitle->setText("FileSubTitle");
}

LSColumnPreview::LSColumnPreview(QWidget *parent)
    : QWidget(parent)
    , p(new LSColumnPreviewPrivate(this))
{}

LSColumnPreview::~LSColumnPreview()
{
    delete p;
}

void LSColumnPreview::updateFileInfo(QFileInfo info)
{
    p->m_fileTitle->setText(info.fileName());
}

