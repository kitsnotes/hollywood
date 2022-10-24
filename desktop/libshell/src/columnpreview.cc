#include "columnpreview.h"

LSColumnPreview::LSColumnPreview(QWidget *parent)
    : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_previewLayout = new QHBoxLayout();
    m_previewSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_previewSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_underPreviewSpacer = new QSpacerItem(20, 15, QSizePolicy::Minimum, QSizePolicy::Maximum);
    m_fileSubTitleLayout = new QHBoxLayout();
    m_fileSubTitleSpacer = new QSpacerItem(10, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_preview = new QGraphicsView(this);
    m_fileTitle = new QLabel(this);
    m_fileSubtitle = new QLabel(this);
    m_fileProperties = new QTableWidget(this);
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

void LSColumnPreview::updateFileInfo(QFileInfo info)
{
    m_fileTitle->setText(info.fileName());
}
