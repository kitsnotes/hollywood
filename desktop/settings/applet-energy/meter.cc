#include "meter.h"
#include "upower.h"

BatteryMeterWidget::BatteryMeterWidget(UPowerDeviceInterface *up, QWidget *parent)
    : QWidget(parent)
    , m_upower(up)
    , m_mainLayout(new QHBoxLayout(this))
    , m_detailLayout(new QVBoxLayout)
    , m_icon(new QLabel(this))
    , m_info(new QLabel(this))
    , m_level(new QProgressBar(this))
    , m_health(new QLabel(this))
{
    auto hl = new QHBoxLayout;
    hl->addWidget(m_info);
    m_detailLayout->addLayout(hl);
    m_detailLayout->addWidget(m_level);
    m_detailLayout->addWidget(m_health);
    m_mainLayout->addWidget(m_icon);
    m_mainLayout->addLayout(m_detailLayout);
    setLayout(m_mainLayout);
    upowerChanged();
    connect(m_upower, &UPowerDeviceInterface::changed,
            this, &BatteryMeterWidget::upowerChanged);
}

BatteryMeterWidget::~BatteryMeterWidget()
{
    disconnect(m_upower, &UPowerDeviceInterface::changed,
               this, &BatteryMeterWidget::upowerChanged);

    m_upower->deleteLater();
}

void BatteryMeterWidget::upowerChanged()
{
    m_info->setText(tr("System Battery: %1").arg(m_upower->stateString()));
    m_icon->setPixmap(QIcon::fromTheme(m_upower->displayIcon()).pixmap(QSize(64,64)));
    m_level->setValue(m_upower->percent());
    auto healthpercent = m_upower->batteryHealthPercent();
    auto health = tr("OK");
    if(healthpercent <= 80)
        health = tr("Degraded");
    m_health->setText(tr("Battery Health: %1 (%2%)").arg(health, QString::number(healthpercent)));
}
