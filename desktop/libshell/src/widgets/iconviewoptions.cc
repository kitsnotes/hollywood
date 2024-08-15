// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "iconviewoptions.h"
#include "iconviewoptions_p.h"


LSIconViewOptionsPrivate::LSIconViewOptionsPrivate(LSIconViewOptions *parent)
    :d(parent)
{
    layout_Main = new QVBoxLayout(parent);
    auto layout_Organize = new QHBoxLayout();
    auto l_Organize = new QLabel(parent);

    layout_Organize->addWidget(l_Organize);

    m_Organize = new QComboBox(parent);
    layout_Organize->addWidget(m_Organize);
    layout_Main->addLayout(layout_Organize);

    auto line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    layout_Main->addWidget(line);

    m_IconSizeLabel = new QLabel(parent);

    layout_Main->addWidget(m_IconSizeLabel);

    auto layout_IconSize = new QHBoxLayout();
    auto l_IconSzSmall = new QLabel(parent);

    layout_IconSize->addWidget(l_IconSzSmall);

    m_IconSizing = new QSlider(parent);
    layout_IconSize->addWidget(m_IconSizing);

    auto l_IconSzLarge = new QLabel(parent);

    layout_IconSize->addWidget(l_IconSzLarge);


    layout_Main->addLayout(layout_IconSize);

    m_GridSpacingLabel = new QLabel(parent);

    layout_Main->addWidget(m_GridSpacingLabel);

    auto horizontalLayout_2 = new QHBoxLayout();
    auto l_GridSzSmall = new QLabel(parent);

    horizontalLayout_2->addWidget(l_GridSzSmall);

    m_GridSizing = new QSlider(parent);
    m_GridSizing->setOrientation(Qt::Horizontal);

    horizontalLayout_2->addWidget(m_GridSizing);

    auto l_GridSzLarge = new QLabel(parent);
    horizontalLayout_2->addWidget(l_GridSzLarge);


    layout_Main->addLayout(horizontalLayout_2);

    auto line_2 = new QFrame(parent);
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    layout_Main->addWidget(line_2);

    auto layout_TextSize = new QHBoxLayout();
    auto l_TextSize = new QLabel(parent);

    layout_TextSize->addWidget(l_TextSize);

    m_TextSize = new QComboBox(parent);
    layout_TextSize->addWidget(m_TextSize);
    layout_TextSize->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));


    layout_Main->addLayout(layout_TextSize);

    auto l_LabelPos = new QLabel(parent);
    layout_Main->addWidget(l_LabelPos);

    auto layout_LabelPos = new QHBoxLayout();
    m_lblBottom = new QRadioButton(parent);

    layout_LabelPos->addWidget(m_lblBottom);

    m_lblRight = new QRadioButton(parent);
    layout_LabelPos->addWidget(m_lblRight);

    layout_LabelPos->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout_Main->addLayout(layout_LabelPos);

    auto line_3 = new QFrame(parent);
    line_3->setObjectName(QString::fromUtf8("line_3"));
    line_3->setFrameShape(QFrame::HLine);
    line_3->setFrameShadow(QFrame::Sunken);
    layout_Main->addWidget(line_3);
    m_iconPreview = new QCheckBox(parent);
    layout_Main->addWidget(m_iconPreview);
    m_itemInfo = new QCheckBox(parent);
    layout_Main->addWidget(m_itemInfo);
    auto layoutDefaults = new QHBoxLayout();
    layoutDefaults->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    m_setDefaults = new QPushButton(parent);
    layoutDefaults->addWidget(m_setDefaults);
    layoutDefaults->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout_Main->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout_Main->addLayout(layoutDefaults);

    l_Organize->setText(QApplication::tr("Organize By:"));
    m_IconSizeLabel->setText(QApplication::tr("Icon Size: 0x0 px"));
    l_IconSzSmall->setText(QApplication::tr("TextLabel"));
    l_IconSzLarge->setText(QApplication::tr("TextLabel"));
    m_GridSpacingLabel->setText(QApplication::tr("Grid Spacing:"));
    l_GridSzSmall->setText(QApplication::tr("TextLabel"));
    l_GridSzLarge->setText(QApplication::tr("TextLabel"));
    l_TextSize->setText(QApplication::tr("Text Size:"));
    l_LabelPos->setText(QApplication::tr("Label Position:"));
    m_lblBottom->setText(QApplication::tr("Bottom"));
    m_lblRight->setText(QApplication::tr("Right"));
    m_iconPreview->setText(QApplication::tr("Show Icon Previews"));
    m_itemInfo->setText(QApplication::tr("Show Item Info"));
    m_setDefaults->setText(QApplication::tr("Set Defaults"));

    m_Organize->addItem(QApplication::tr("Name"));
    m_Organize->addItem(QApplication::tr("File Type"));
    m_Organize->addItem(QApplication::tr("Size"));
    m_Organize->addItem(QApplication::tr("Date Created"));
    m_Organize->addItem(QApplication::tr("Date Last Modified"));

    m_IconSizing->setOrientation(Qt::Horizontal);
    m_IconSizing->setMinimum(16);
    m_IconSizing->setMaximum(128);
    m_IconSizing->setSingleStep(4);
    m_IconSizing->setPageStep(8);
    m_IconSizing->setTickInterval(8);
    m_IconSizing->setTickPosition(QSlider::TicksBelow);

    QIcon fileIcon = QIcon::fromTheme("text-x-generic");
    QPixmap smallIcon = fileIcon.pixmap(QSize(16,16));
    QPixmap largeIcon = fileIcon.pixmap(QSize(32,32));

    l_IconSzSmall->setPixmap(smallIcon);
    l_IconSzLarge->setPixmap(largeIcon);
    l_IconSzSmall->setMaximumSize(QSize(40,40));
    l_IconSzLarge->setMaximumSize(QSize(40,40));

    QIcon smGridIcon = QIcon::fromTheme("games-config-board");
    QIcon lgGridIcon = QIcon::fromTheme("map-flat");
    smallIcon = smGridIcon.pixmap(QSize(16,16));
    largeIcon = lgGridIcon.pixmap(QSize(20,20));

    m_GridSizing->setMinimum(1);
    m_GridSizing->setMaximum(20);
    m_GridSizing->setSingleStep(1);
    m_GridSizing->setPageStep(2);
    m_GridSizing->setTickInterval(2);
    m_GridSizing->setTickPosition(QSlider::TicksBelow);

    l_GridSzSmall->setPixmap(smallIcon);
    l_GridSzLarge->setPixmap(largeIcon);
    l_GridSzLarge->setMaximumSize(QSize(40,40));
    l_GridSzSmall->setMaximumSize(QSize(40,40));

    m_TextSize->addItem("10");
    m_TextSize->addItem("11");
    m_TextSize->addItem("12");
    m_TextSize->addItem("13");
    m_TextSize->addItem("14");
    m_TextSize->addItem("15");
    m_TextSize->addItem("16");
}

LSIconViewOptions::LSIconViewOptions(QWidget *parent)
    : QWidget(parent)
    , p(new LSIconViewOptionsPrivate(this))
{
    setLayout(p->layout_Main);
}

void LSIconViewOptions::attachView(QListView *view)
{
    p->m_view = view;
}

void LSIconViewOptions::refreshViewOptions()
{
    if(p->m_view == nullptr)
        return;

    QSize iconSize = p->m_view->iconSize();
    p->m_IconSizing->setValue(iconSize.width());
    p->m_IconSizeLabel->setText(QApplication::tr("Icon Size: %1x%2 px")
                    .arg(p->m_view->iconSize().height())
                    .arg(p->m_view->iconSize().width()));

    p->m_GridSpacingLabel->setText(QApplication::tr("Grid Spacing: %1 px").arg(p->m_view->spacing()));
    p->m_GridSizing->setValue(p->m_view->spacing());

    /* switch(view->flow())
    {
    case QListView::LeftToRight:
        m_
    } */
}
