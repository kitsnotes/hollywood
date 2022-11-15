#include "iconviewoptions.h"


IconViewOptions::IconViewOptions(QWidget *parent)
    : QWidget(parent),
      m_view(nullptr)
{
    layout_Main = new QVBoxLayout(this);
    setLayout(layout_Main);
    setupWidget();

    m_Organize->addItem(tr("Name"));
    m_Organize->addItem(tr("File Type"));
    m_Organize->addItem(tr("Size"));
    m_Organize->addItem(tr("Date Created"));
    m_Organize->addItem(tr("Date Last Modified"));

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

void IconViewOptions::setupWidget()
{
    layout_Organize = new QHBoxLayout();

    l_Organize = new QLabel(this);
    l_Organize->setObjectName(QString::fromUtf8("l_Organize"));

    layout_Organize->addWidget(l_Organize);

    m_Organize = new QComboBox(this);
    m_Organize->setObjectName(QString::fromUtf8("m_Organize"));

    layout_Organize->addWidget(m_Organize);


    layout_Main->addLayout(layout_Organize);

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    layout_Main->addWidget(line);

    l_IconSize = new QLabel(this);
    l_IconSize->setObjectName(QString::fromUtf8("l_IconSize"));

    layout_Main->addWidget(l_IconSize);

    layout_IconSize = new QHBoxLayout();
    layout_IconSize->setObjectName(QString::fromUtf8("layout_IconSize"));
    l_IconSzSmall = new QLabel(this);
    l_IconSzSmall->setObjectName(QString::fromUtf8("l_IconSzSmall"));

    layout_IconSize->addWidget(l_IconSzSmall);

    m_IconSizing = new QSlider(this);
    m_IconSizing->setObjectName(QString::fromUtf8("m_IconSizing"));
    layout_IconSize->addWidget(m_IconSizing);

    l_IconSzLarge = new QLabel(this);
    l_IconSzLarge->setObjectName(QString::fromUtf8("l_IconSzLarge"));

    layout_IconSize->addWidget(l_IconSzLarge);


    layout_Main->addLayout(layout_IconSize);

    l_GridSpacing = new QLabel(this);
    l_GridSpacing->setObjectName(QString::fromUtf8("l_GridSpacing"));

    layout_Main->addWidget(l_GridSpacing);

    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    l_GridSzSmall = new QLabel(this);
    l_GridSzSmall->setObjectName(QString::fromUtf8("l_GridSzSmall"));

    horizontalLayout_2->addWidget(l_GridSzSmall);

    m_GridSizing = new QSlider(this);
    m_GridSizing->setObjectName(QString::fromUtf8("m_GridSizing"));
    m_GridSizing->setOrientation(Qt::Horizontal);

    horizontalLayout_2->addWidget(m_GridSizing);

    l_GridSzLarge = new QLabel(this);
    l_GridSzLarge->setObjectName(QString::fromUtf8("l_GridSzLarge"));

    horizontalLayout_2->addWidget(l_GridSzLarge);


    layout_Main->addLayout(horizontalLayout_2);

    line_2 = new QFrame(this);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setFrameShape(QFrame::HLine);
    line_2->setFrameShadow(QFrame::Sunken);

    layout_Main->addWidget(line_2);

    layout_TextSize = new QHBoxLayout();
    layout_TextSize->setObjectName(QString::fromUtf8("layout_TextSize"));
    l_TextSize = new QLabel(this);
    l_TextSize->setObjectName(QString::fromUtf8("l_TextSize"));

    layout_TextSize->addWidget(l_TextSize);

    m_TextSize = new QComboBox(this);
    m_TextSize->setObjectName(QString::fromUtf8("m_TextSize"));

    layout_TextSize->addWidget(m_TextSize);

    spTextSize = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout_TextSize->addItem(spTextSize);


    layout_Main->addLayout(layout_TextSize);

    l_LabelPos = new QLabel(this);
    l_LabelPos->setObjectName(QString::fromUtf8("l_LabelPos"));

    layout_Main->addWidget(l_LabelPos);

    layout_LabelPos = new QHBoxLayout();
    layout_LabelPos->setObjectName(QString::fromUtf8("layout_LabelPos"));
    m_lblBottom = new QRadioButton(this);
    m_lblBottom->setObjectName(QString::fromUtf8("m_lblBottom"));

    layout_LabelPos->addWidget(m_lblBottom);

    m_lblRight = new QRadioButton(this);
    m_lblRight->setObjectName(QString::fromUtf8("m_lblRight"));

    layout_LabelPos->addWidget(m_lblRight);

    spLabelPos = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout_LabelPos->addItem(spLabelPos);
    layout_Main->addLayout(layout_LabelPos);

    line_3 = new QFrame(this);
    line_3->setObjectName(QString::fromUtf8("line_3"));
    line_3->setFrameShape(QFrame::HLine);
    line_3->setFrameShadow(QFrame::Sunken);

    layout_Main->addWidget(line_3);

    checkBox = new QCheckBox(this);
    checkBox->setObjectName(QString::fromUtf8("checkBox"));

    layout_Main->addWidget(checkBox);

    checkBox_2 = new QCheckBox(this);
    checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
    layout_Main->addWidget(checkBox_2);
    layoutDefaults = new QHBoxLayout();
    layoutDefaults->setObjectName(QString::fromUtf8("LayoutDefauls"));
    sp_DefLeft = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layoutDefaults->addItem(sp_DefLeft);
    m_setDefaults = new QPushButton(this);
    m_setDefaults->setObjectName(QString::fromUtf8("SetDefaults"));
    layoutDefaults->addWidget(m_setDefaults);
    sp_DefRight = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layoutDefaults->addItem(sp_DefRight);
    spBottom = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout_Main->addItem(spBottom);
    layout_Main->addLayout(layoutDefaults);

    l_Organize->setText(tr("Organize By:"));
    l_IconSize->setText(tr("Icon Size: 0x0 px"));
    l_IconSzSmall->setText(tr("TextLabel"));
    l_IconSzLarge->setText(tr("TextLabel"));
    l_GridSpacing->setText(tr("Grid Spacing:"));
    l_GridSzSmall->setText(tr("TextLabel"));
    l_GridSzLarge->setText(tr("TextLabel"));
    l_TextSize->setText(tr("Text Size:"));
    l_LabelPos->setText(tr("Label Position:"));
    m_lblBottom->setText(tr("Bottom"));
    m_lblRight->setText(tr("Right"));
    checkBox->setText(tr("Show Icon Previews"));
    checkBox_2->setText(tr("Show Item Info"));
    m_setDefaults->setText(tr("Set Defaults"));
}

void IconViewOptions::attachView(QListView *view)
{
    qDebug() << "IconViewOptions::attachView";
    m_view = view;
}

void IconViewOptions::refreshViewOptions()
{
    if(m_view == nullptr)
        return;

    qDebug() << "Refreshing Icon View Options";
    QSize iconSize = m_view->iconSize();
    m_IconSizing->setValue(iconSize.width());
    l_IconSize->setText(tr("Icon Size: %1x%2 px")
                    .arg(m_view->iconSize().height())
                    .arg(m_view->iconSize().width()));

    l_GridSpacing->setText(tr("Grid Spacing: %1 px").arg(m_view->spacing()));
    m_GridSizing->setValue(m_view->spacing());

    /* switch(view->flow())
    {
    case QListView::LeftToRight:
        m_
    } */
}
