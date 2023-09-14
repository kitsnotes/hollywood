#include "meter.h"
#include "upower.h"

BatteryMeterWidget::BatteryMeterWidget(UPowerDeviceInterface *up, QWidget *parent)
    : QWidget(parent)
    , m_upower(up)
    , m_mainLayout(new QHBoxLayout(this))
    , m_detailLayout(new QVBoxLayout)
    , m_icon(new QLabel(this))
    , m_level(new QProgressBar(this))
{
    m_detailLayout->addWidget(m_level);
    m_mainLayout->addWidget(m_icon);
    m_mainLayout->addLayout(m_detailLayout);
    setLayout(m_mainLayout);
    upowerChanged();
}

void BatteryMeterWidget::upowerChanged()
{
    m_icon->setPixmap(QIcon::fromTheme(m_upower->displayIcon()).pixmap(QSize(64,64)));
    m_level->setValue(m_upower->percent());
}
