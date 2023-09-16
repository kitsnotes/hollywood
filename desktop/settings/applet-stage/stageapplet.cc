#include "stageapplet.h"
#include <QSettings>
#include <hollywood/hollywood.h>

StageApplet::StageApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool StageApplet::init()
{
    setupWidget();
    loadSettings();
    return true;
}

bool StageApplet::loadSettings()
{
    QSettings settings("originull","hollywood");
    settings.beginGroup("Stage");
    auto layout = settings.value("UseSouthernMode", HOLLYWOOD_STAGE_LAYOUT).toBool();
    if(layout == 0)
        m_northern->setChecked(true);
    else
        m_southern->setChecked(true);

    layoutSelectionChanged();

    auto size = settings.value("Size", HOLLYWOOD_STAGE_SIZE).toUInt();
    if(size == 0)
        m_small->setChecked(true);
    else
        m_large->setChecked(true);

    m_selectedPosition = (Qt::Edge)settings.value("Position", HOLLYWOOD_STAGE_POSITION).toUInt();

    updatePositionOptions();

    auto show_clock = settings.value("ShowClock", HOLLYWOOD_STCLK_SHOW).toBool();
    auto show_date = settings.value("ShowDateInClock", HOLLYWOOD_STCLK_USEDATE).toBool();
    auto show_seconds = settings.value("ShowSecondsInClock", HOLLYWOOD_STCLK_USESECONDS).toBool();
    auto clock_24hr = settings.value("Use24HourClock", HOLLYWOOD_STCLK_24HOUR).toBool();
    auto clock_ampm = settings.value("ShowAMPMInClock", HOLLYWOOD_STCLK_USEAMPM).toBool();

    m_showclock->setChecked(show_clock);
    m_showdate->setChecked(show_date);
    m_seconds->setChecked(show_seconds);
    m_24hour->setChecked(clock_24hr);
    showClockChecked();

    return true;
}

bool StageApplet::saveSettings()
{
    qDebug() << "saveSettings";
    QSettings settings("originull","hollywood");
    settings.beginGroup("Stage");
    settings.setValue("ShowClock", m_showclock->isChecked());
    settings.setValue("ShowDateInClock", m_showdate->isChecked());
    settings.setValue("ShowSecondsInClock", m_seconds->isChecked());
    settings.setValue("Use24HourClock", m_24hour->isChecked());
    settings.setValue("ShowAMPMInClock", m_seconds->isChecked());

    if(m_southern->isChecked())
        settings.setValue("UseSouthernMode", true);
    else
        settings.setValue("UseSouthernMode", false);
    return true;
}

QString StageApplet::id() const
{
    return QLatin1String("org.originull.stage");
}

QString StageApplet::name() const
{
    return tr("Stage & Menu Bar");
}

QString StageApplet::description() const
{
    return tr("Configure stage and menu bar settings.");
}

QIcon StageApplet::icon() const
{
    return QIcon::fromTheme("preferences-desktop-activities");
}

QWidget *StageApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category StageApplet::category() const
{
    return Personal;
}

QStringList StageApplet::searchTokens() const
{
    return QStringList();
}

void StageApplet::layoutSelectionChanged()
{
    if(m_layout->checkedButton() == m_northern)
    {
        // We are northern (Redmond) style
        m_layoutdesc->setText(tr("A desktop layout from Redmond, Washington."));
    }
    else
    {
        // we are southern (Cupertino) style
        m_layoutdesc->setText(tr("A desktop layout from Cupertino, California."));
    }
    updatePositionOptions();
    saveSettings();
}

void StageApplet::stageSizeChanged()
{

}

void StageApplet::showClockChecked()
{
    m_showdate->setEnabled(m_showclock->isChecked());
    m_seconds->setEnabled(m_showclock->isChecked());
    m_24hour->setEnabled(m_showclock->isChecked());
}

void StageApplet::updatePositionOptions()
{
    disconnect(m_position, &QComboBox::currentIndexChanged, this, &StageApplet::layoutIndexChanged);
    m_position->clear();
    if(m_southern->isChecked())
    {
        // if we are using cupertino style we
        // can't dock the stage at the top
        if(m_selectedPosition == Qt::TopEdge)
            m_selectedPosition = Qt::BottomEdge;
    }

    if(m_northern->isChecked())
        m_position->addItem(tr("Top"), Qt::TopEdge);

    m_position->addItem(tr("Left"), Qt::LeftEdge);
    m_position->addItem(tr("Right"), Qt::RightEdge);
    m_position->addItem(tr("Bottom"), Qt::BottomEdge);

    switch(m_selectedPosition)
    {
    case Qt::TopEdge:
        m_position->setCurrentIndex(0);
        break;
    case Qt::LeftEdge:
        if(m_northern->isChecked())
            m_position->setCurrentIndex(1);
        else
            m_position->setCurrentIndex(0);
    case Qt::RightEdge:
        if(m_northern->isChecked())
            m_position->setCurrentIndex(2);
        else
            m_position->setCurrentIndex(1);
    case Qt::BottomEdge:
    default:
        if(m_northern->isChecked())
            m_position->setCurrentIndex(3);
        else
            m_position->setCurrentIndex(2);
        break;
    }
    connect(m_position, &QComboBox::currentIndexChanged, this, &StageApplet::layoutIndexChanged);
}

void StageApplet::layoutIndexChanged(int index)
{
    if(m_southern->isChecked())
    {
        switch(index)
        {
        case 0:
            m_selectedPosition = Qt::LeftEdge;
            break;
        case 1:
            m_selectedPosition = Qt::RightEdge;
            break;
        case 2:
        default:
            m_selectedPosition = Qt::BottomEdge;
            break;
        }
    }
    else
    {
        switch(index)
        {
        case 0:
            m_selectedPosition = Qt::TopEdge;
            break;
        case 1:
            m_selectedPosition = Qt::LeftEdge;
            break;
        case 2:
            m_selectedPosition = Qt::RightEdge;
            break;
        case 3:
        default:
            m_selectedPosition = Qt::BottomEdge;
            break;
        }
    }
}

void StageApplet::setupWidget()
{
    m_host = new QWidget(0);
    mainLayout = new QHBoxLayout(m_host);
    m_layout = new QButtonGroup(m_host);
    m_stagesize = new QButtonGroup(m_host);
    connect(m_layout, &QButtonGroup::buttonToggled, this, &StageApplet::layoutSelectionChanged);
    auto ml_hs_left = new QSpacerItem(150, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    mainLayout->addItem(ml_hs_left);

    auto fl_main = new QFormLayout();
    fl_main->setObjectName(QString::fromUtf8("fl_main"));
    fl_main->setSizeConstraint(QLayout::SetDefaultConstraint);
    fl_main->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    lb_layoutdesc = new QLabel(m_host);
    lb_layoutdesc->setObjectName(QString::fromUtf8("lb_layoutdesc"));

    fl_main->setWidget(0, QFormLayout::LabelRole, lb_layoutdesc);

    auto vl_ns = new QVBoxLayout();
    auto hl_ns = new QHBoxLayout();
    m_northern = new QRadioButton(m_host);
    m_southern = new QRadioButton(m_host);

    m_layout->addButton(m_northern);
    m_layout->addButton(m_southern);

    hl_ns->addWidget(m_northern);
    hl_ns->addWidget(m_southern);
    vl_ns->addLayout(hl_ns);

    m_layoutdesc = new QLabel(m_host);
    auto f = m_layoutdesc->font();
    f.setPointSize(f.pointSize()-2);
    m_layoutdesc->setFont(f);
    vl_ns->addWidget(m_layoutdesc);

    auto lb_stagesize = new QLabel(m_host);
    auto hl_stage_size = new QHBoxLayout();
    m_small = new QRadioButton(m_host);
    m_large = new QRadioButton(m_host);
    connect(m_small, &QPushButton::pressed, this, &StageApplet::saveSettings);
    connect(m_large, &QPushButton::pressed, this, &StageApplet::saveSettings);

    m_stagesize->addButton(m_small);
    m_stagesize->addButton(m_large);
    auto label = new QLabel(m_host);
    m_position = new QComboBox(m_host);

    hl_stage_size->addWidget(m_small);
    hl_stage_size->addWidget(m_large);

    connect(m_position, &QComboBox::currentIndexChanged, this, &StageApplet::layoutIndexChanged);

    line = new QFrame(m_host);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    m_showclock = new QCheckBox(m_host);

    connect(m_showclock, &QCheckBox::toggled, this, &StageApplet::showClockChecked);
    m_showdate = new QCheckBox(m_host);

    fl_main->setLayout(0, QFormLayout::FieldRole, vl_ns);
    fl_main->setWidget(1, QFormLayout::LabelRole, lb_stagesize);
    fl_main->setLayout(1, QFormLayout::FieldRole, hl_stage_size);
    fl_main->setWidget(2, QFormLayout::LabelRole, label);
    fl_main->setWidget(2, QFormLayout::FieldRole, m_position);
    fl_main->setWidget(3, QFormLayout::SpanningRole, line);
    fl_main->setWidget(4, QFormLayout::FieldRole, m_showclock);
    fl_main->setWidget(5, QFormLayout::FieldRole, m_showdate);

    m_seconds = new QCheckBox(m_host);

    fl_main->setWidget(6, QFormLayout::FieldRole, m_seconds);

    m_24hour = new QCheckBox(m_host);
    lbl_clock = new QLabel(m_host);

    fl_main->setWidget(7, QFormLayout::FieldRole, m_24hour);
    fl_main->setItem(8, QFormLayout::FieldRole, new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    fl_main->setWidget(4, QFormLayout::LabelRole, lbl_clock);

    connect(m_showclock, &QCheckBox::toggled, this, &StageApplet::saveSettings);
    connect(m_showdate, &QCheckBox::toggled, this, &StageApplet::saveSettings);
    connect(m_seconds, &QCheckBox::toggled, this, &StageApplet::saveSettings);
    connect(m_24hour, &QCheckBox::toggled, this, &StageApplet::saveSettings);

    mainLayout->addLayout(fl_main);
    mainLayout->addItem(new QSpacerItem(149, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    lbl_clock->setText(QCoreApplication::translate("StageSettings", "Clock Settings:", nullptr));
    lb_layoutdesc->setText(QCoreApplication::translate("StageSettings", "Layout:", nullptr));
    m_northern->setText(QCoreApplication::translate("StageSettings", "Northern", nullptr));
    m_southern->setText(QCoreApplication::translate("StageSettings", "Southern", nullptr));
    m_layoutdesc->setText(QCoreApplication::translate("StageSettings", "TextLabel", nullptr));
    lb_stagesize->setText(QCoreApplication::translate("StageSettings", "Stage Size:", nullptr));
    m_small->setText(QCoreApplication::translate("StageSettings", "Small", nullptr));
    m_large->setText(QCoreApplication::translate("StageSettings", "Large", nullptr));
    label->setText(QCoreApplication::translate("StageSettings", "Stage Position:", nullptr));
    m_showclock->setText(QCoreApplication::translate("StageSettings", "Show clock in the stage.", nullptr));
    m_showdate->setText(QCoreApplication::translate("StageSettings", "Show date in clock.", nullptr));
    m_seconds->setText(QCoreApplication::translate("StageSettings", "Show time with seconds.", nullptr));
    m_24hour->setText(QCoreApplication::translate("StageSettings", "Use a 24 hour clock.", nullptr));

    m_southern->setEnabled(false);
    m_large->setEnabled(false);
}

