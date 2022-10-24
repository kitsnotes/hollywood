#include "mouseapplet.h"
#include <QSettings>
#include <hollywood/hollywood.h>

MouseApplet::MouseApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool MouseApplet::init()
{
    setupWidget();
    loadSettings();
    connect(m_dblclk, &QSlider::sliderReleased, this, &MouseApplet::widgetUpdate);
    connect(m_scroll, &QSlider::sliderReleased, this, &MouseApplet::widgetUpdate);
    connect(m_left, &QRadioButton::toggled, this, &MouseApplet::widgetUpdate);
    connect(m_right, &QRadioButton::toggled, this, &MouseApplet::widgetUpdate);
    connect(m_scrolldir, &QCheckBox::toggled, this, &MouseApplet::widgetUpdate);
    return true;
}

bool MouseApplet::loadSettings()
{
    QSettings settings("originull","hollywood");
    settings.beginGroup("Mouse");
    auto scroll = settings.value("MouseScrollSpeed", HOLLYWOOD_DEF_SCROLL_SPEED).toUInt();
    if(scroll < 1)
        scroll = 1;
    if(scroll > HOLLYWOOD_MAX_SCROLL_SPEED)
        scroll = HOLLYWOOD_MAX_SCROLL_SPEED;

    auto dblclk = settings.value("DoubleClickSpeed", HOLLYWOOD_DEF_DBLCLK_TIME).toUInt();
    if(dblclk > HOLLYWOOD_MAX_DBLCLK_TIME)
        dblclk = HOLLYWOOD_MAX_DBLCLK_TIME;
    if(dblclk < HOLLYWOOD_MIN_DBLCLK_TIME)
        dblclk = HOLLYWOOD_MIN_DBLCLK_TIME;
    uint primaryBtn = settings.value("PrimaryButton", 0).toUInt();
    if(primaryBtn > 1)
        primaryBtn = 0;
    auto ns = settings.value("NaturalScroll", false).toBool();
    settings.endGroup();

    m_scroll->setValue(scroll);
    m_dblclk->setValue(0-dblclk);
    if(primaryBtn == 1)
        m_right->setChecked(true);
    else
        m_left->setChecked(true);

    m_scrolldir->setChecked(ns);
    return true;
}

bool MouseApplet::saveSettings()
{
    QSettings settings("originull","hollywood");
    settings.beginGroup("Mouse");
    settings.setValue("MouseScrollSpeed", m_scroll->value());
    settings.setValue("DoubleClickSpeed", abs(m_dblclk->value()));
    uint primaryBtn = 0;
    if(m_right->isChecked())
        primaryBtn = 1;
    settings.setValue("PrimaryButton", primaryBtn);
    settings.setValue("NaturalScroll", m_scrolldir->isChecked());

    return true;
}

QString MouseApplet::id() const
{
    return QLatin1String("org.originull.mouse");
}

QString MouseApplet::name() const
{
    return tr("Mouse");
}

QString MouseApplet::description() const
{
    return tr("Mouse Settings");
}

QIcon MouseApplet::icon() const
{
    const QIcon mine(QIcon::fromTheme("preferences-desktop-mouse"));
    return mine;
}

QWidget *MouseApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category MouseApplet::category() const
{
    return System;
}

void MouseApplet::widgetUpdate()
{
    saveSettings();
}

void MouseApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("MouseAppletHost"));

    auto hl = new QHBoxLayout(m_host);
    auto hsleft = new QSpacerItem(90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vlmain = new QVBoxLayout();
    auto flForm = new QFormLayout();
    auto lbscroll = new QLabel(m_host);

    auto vlScrollSpeed = new QVBoxLayout();
    auto hl_scroll = new QHBoxLayout();
    auto l_dslow = new QLabel(m_host);
    auto l_rfast_2 = new QLabel(m_host);
    auto lbdbclk = new QLabel(m_host);
    auto vl_dblclk = new QVBoxLayout();
    auto hl_dblclk = new QHBoxLayout();
    auto l_dslow_2 = new QLabel(m_host);
    auto l_rfast_3 = new QLabel(m_host);
    auto lbprimary = new QLabel(m_host);
    auto vlPrimaryButton = new QVBoxLayout();
    m_scroll = new QSlider(m_host);
    m_dblclk = new QSlider(m_host);
    m_left = new QRadioButton(m_host);
    m_leftright = new QButtonGroup(m_host);
    m_right = new QRadioButton(m_host);
    m_scrolldir = new QCheckBox(m_host);
    vsend = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    hsright = new QSpacerItem(89, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_scroll->setMinimumSize(QSize(120, 0));
    m_scroll->setOrientation(Qt::Horizontal);
    m_scroll->setTickPosition(QSlider::TicksBothSides);

    l_rfast_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    vlScrollSpeed->addWidget(m_scroll);
    hl_scroll->addWidget(l_dslow);
    hl_scroll->addWidget(l_rfast_2);
    vlScrollSpeed->addLayout(hl_scroll);

    flForm->setWidget(0, QFormLayout::LabelRole, lbscroll);
    flForm->setLayout(0, QFormLayout::FieldRole, vlScrollSpeed);
    flForm->setWidget(1, QFormLayout::LabelRole, lbdbclk);

    m_dblclk->setMinimumSize(QSize(120, 0));
    m_dblclk->setOrientation(Qt::Horizontal);
    m_dblclk->setTickPosition(QSlider::TicksBothSides);
    l_rfast_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hl_dblclk->addWidget(l_dslow_2);
    hl_dblclk->addWidget(l_rfast_3);
    vl_dblclk->addWidget(m_dblclk);
    vl_dblclk->addLayout(hl_dblclk);

    m_leftright->addButton(m_left);
    m_leftright->addButton(m_right);

    vlPrimaryButton->addWidget(m_left);
    vlPrimaryButton->addWidget(m_right);

    flForm->setLayout(1, QFormLayout::FieldRole, vl_dblclk);
    flForm->setWidget(2, QFormLayout::LabelRole, lbprimary);
    flForm->setLayout(2, QFormLayout::FieldRole, vlPrimaryButton);

    vlmain->addLayout(flForm);
    vlmain->addWidget(m_scrolldir);
    vlmain->addItem(vsend);

    hl->addItem(hsleft);
    hl->addLayout(vlmain);
    hl->addItem(hsright);

    lbscroll->setText(tr("Scroll Speed:"));
    l_dslow->setText(tr("Slow"));
    l_rfast_2->setText(tr("Fast"));
    lbdbclk->setText(tr("Double Click Speed:"));
    l_dslow_2->setText(tr("Slow"));
    l_rfast_3->setText(tr("Fast"));
    lbprimary->setText(tr("Primary Button:"));
    m_left->setText(tr("Left"));
    m_right->setText(tr("Right"));
    m_scrolldir->setText(tr("Use natural scroll direction"));

    m_scroll->setRange(1, HOLLYWOOD_MAX_SCROLL_SPEED);
    m_scroll->setTickInterval(6);
    m_scroll->setPageStep(3);

    auto min = 0 - HOLLYWOOD_MIN_DBLCLK_TIME;
    auto max = 0 - HOLLYWOOD_MAX_DBLCLK_TIME;
    m_dblclk->setRange(max,min);
    m_dblclk->setTickInterval(300);
    m_dblclk->setPageStep(100);

}

