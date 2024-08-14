#include "desktopviewoptions.h"
#include <QApplication>

DesktopViewOptions::DesktopViewOptions(QWidget *parent)
    :QDialog(parent)
{
    setWindowTitle(tr("View Options: %1").arg(tr("Desktop")));
    layout_Main = new QVBoxLayout(this);
    m_Organize = new QComboBox(this);
    m_IconSizeLabel = new QLabel(this);
    m_IconSizing = new QSlider(this);
    m_GridSpacingLabel = new QLabel(this);
    m_GridSizing = new QSlider(this);
    m_TextSize = new QComboBox(this);
    m_lblBottom = new QRadioButton(this);
    m_lblRight = new QRadioButton(this);
    m_iconPreview = new QCheckBox(this);
    m_itemInfo = new QCheckBox(this);
    m_setDefaults = new QPushButton(this);

    m_Organize->setDisabled(true);
    m_iconPreview->setDisabled(true);
    m_itemInfo->setDisabled(true);
    m_setDefaults->setDisabled(true);
    m_lblBottom->setDisabled(true);
    m_lblRight->setDisabled(true);

    connect(m_IconSizing, &QSlider::valueChanged, this, &DesktopViewOptions::optionsChanged);
    connect(m_GridSizing, &QSlider::valueChanged, this, &DesktopViewOptions::optionsChanged);
    connect(m_IconSizing, &QSlider::valueChanged, this, &DesktopViewOptions::updateIconSizeLabel);
    connect(m_GridSizing, &QSlider::valueChanged, this, &DesktopViewOptions::updateSpacingLabel);
    connect(m_lblBottom, &QAbstractButton::pressed, this, &DesktopViewOptions::optionsChanged);
    connect(m_lblRight, &QAbstractButton::pressed, this, &DesktopViewOptions::optionsChanged);
    connect(m_iconPreview, &QAbstractButton::pressed, this, &DesktopViewOptions::optionsChanged);
    connect(m_itemInfo, &QAbstractButton::pressed, this, &DesktopViewOptions::optionsChanged);

    auto layout_Organize = new QHBoxLayout();
    auto l_Organize = new QLabel(this);
    auto line = new QFrame(this);
    auto layout_IconSize = new QHBoxLayout();
    auto l_IconSzSmall = new QLabel(this);
    auto l_IconSzLarge = new QLabel(this);
    auto horizontalLayout_2 = new QHBoxLayout();
    auto l_GridSzSmall = new QLabel(this);
    auto l_GridSzLarge = new QLabel(this);
    auto line_2 = new QFrame(this);
    auto layout_TextSize = new QHBoxLayout();
    auto l_TextSize = new QLabel(this);
    auto l_LabelPos = new QLabel(this);
    auto layout_LabelPos = new QHBoxLayout();
    auto line_3 = new QFrame(this);
    auto layoutDefaults = new QHBoxLayout();
    layout_Organize->addWidget(l_Organize);
    layout_Organize->addWidget(m_Organize);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout_IconSize->addWidget(l_IconSzSmall);
    layout_IconSize->addWidget(m_IconSizing);
    layout_IconSize->addWidget(l_IconSzLarge);

    m_GridSizing->setOrientation(Qt::Horizontal);

    horizontalLayout_2->addWidget(l_GridSzSmall);
    horizontalLayout_2->addWidget(m_GridSizing);
    horizontalLayout_2->addWidget(l_GridSzLarge);

    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    layout_TextSize->addWidget(l_TextSize);
    layout_TextSize->addWidget(m_TextSize);
    layout_TextSize->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout_LabelPos->addWidget(m_lblBottom);
    layout_LabelPos->addWidget(m_lblRight);
    layout_LabelPos->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    line_3->setObjectName(QString::fromUtf8("line_3"));
    line_3->setFrameShape(QFrame::HLine);
    line_3->setFrameShadow(QFrame::Sunken);
    layoutDefaults->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layoutDefaults->addWidget(m_setDefaults);
    layoutDefaults->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout_Main->addLayout(layout_Organize);
    layout_Main->addWidget(line);
    layout_Main->addWidget(m_IconSizeLabel);
    layout_Main->addLayout(layout_IconSize);
    layout_Main->addWidget(m_GridSpacingLabel);
    layout_Main->addLayout(horizontalLayout_2);
    layout_Main->addWidget(line_2);
    layout_Main->addLayout(layout_TextSize);
    layout_Main->addWidget(l_LabelPos);
    layout_Main->addLayout(layout_LabelPos);
    layout_Main->addWidget(line_3);
    layout_Main->addWidget(m_iconPreview);
    layout_Main->addWidget(m_itemInfo);
    layout_Main->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout_Main->addLayout(layoutDefaults);

    l_Organize->setText(QApplication::tr("Organize By:"));
    m_IconSizeLabel->setText(QApplication::tr("Icon Size: 0x0 px"));
    m_GridSpacingLabel->setText(QApplication::tr("Grid Spacing:"));
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

void DesktopViewOptions::updateViewOptions(QListView *view)
{
    m_GridSpacingLabel->setText(QApplication::tr("Grid Spacing: %1 px").arg(QString::number(view->spacing())));
    m_GridSizing->setValue(view->spacing());
    m_IconSizeLabel->setText(QApplication::tr("Icon Size: %1x%1 px").arg(QString::number(view->iconSize().height())));
    m_IconSizing->setValue(view->iconSize().height());
    int idx = view->font().pointSize()-10;
    if(idx < 1)
        idx = 1;

    if(idx > 6)
        idx = 6;

    m_TextSize->setCurrentIndex(idx);
    m_lblBottom->setChecked(true);
}

uint DesktopViewOptions::iconSize() const
{
    return m_IconSizing->value();
}

uint DesktopViewOptions::spacing() const
{
    return m_GridSizing->value();
}

uint DesktopViewOptions::textsize() const
{
    uint fontsize = m_TextSize->currentIndex();
    return fontsize+10;
}

void DesktopViewOptions::updateIconSizeLabel()
{
    m_IconSizeLabel->setText(QApplication::tr("Icon Size: %1x%1 px").arg(QString::number(m_IconSizing->value())));
}

void DesktopViewOptions::updateSpacingLabel()
{
    m_GridSpacingLabel->setText(QApplication::tr("Grid Spacing: %1 px").arg(QString::number(m_GridSizing->value())));
}
