#include "duckapplet.h"

DuckApplet::DuckApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool DuckApplet::init()
{
    setupWidget();
    return true;
}

QString DuckApplet::id() const
{
    return QLatin1String("org.arionos.duck");
}

QString DuckApplet::name() const
{
    return tr("Duck");
}

QString DuckApplet::description() const
{
    return tr("Mr. Duck is awesome");
}

QIcon DuckApplet::icon() const
{
    const QIcon mine(":/Duck/Icon");
    return mine;
}

QWidget *DuckApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category DuckApplet::category() const
{
    return Personal;
}

void DuckApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("DuckAppletHost"));
    m_host->resize(625, 439);

    mainLayout = new QVBoxLayout(m_host);
    mainLayout->setObjectName(QString::fromUtf8("verticalLayout_3"));
    m_topLayout = new QHBoxLayout();
    m_topLayout->setObjectName(QString::fromUtf8("m_topLayout"));
    m_duckLabel = new QLabel(m_host);
    m_duckLabel->setObjectName(QString::fromUtf8("m_duckLabel"));
    m_duckLabel->setPixmap(QPixmap(QString::fromUtf8(":Duck/Mallard")));
    m_duckLabel->setMinimumHeight(127);
    m_duckLabel->setMinimumWidth(150);
    m_duckLabel->setMaximumWidth(150);
    m_duckLabel->setScaledContents(false);

    m_topLayout->addWidget(m_duckLabel);

    m_rightSideLayout = new QVBoxLayout();
    m_rightSideLayout->setObjectName(QString::fromUtf8("m_rightSideLayout"));
    m_welcome = new QLabel(m_host);
    m_welcome->setObjectName(QString::fromUtf8("m_welcome"));

    m_rightSideLayout->addWidget(m_welcome);

    m_description = new QLabel(m_host);
    m_description->setObjectName(QString::fromUtf8("m_description"));
    m_description->setTextFormat(Qt::PlainText);
    m_description->setWordWrap(true);
    m_rightSideLayout->addWidget(m_description);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);


    m_rightSideLayout->addItem(verticalSpacer);
    m_topLayout->addLayout(m_rightSideLayout);


    mainLayout->addLayout(m_topLayout);


    m_optionLayout = new QVBoxLayout();
    m_optionLayout->setObjectName(QString::fromUtf8("m_optionLayout"));
    m_quackMidnight = new QCheckBox(m_host);
    m_quackMidnight->setObjectName(QString::fromUtf8("m_quackMidnight"));
    m_optionLayout->addWidget(m_quackMidnight);
    m_quackHourly = new QCheckBox(m_host);
    m_quackHourly->setObjectName(QString::fromUtf8("m_quackHourly"));
    m_optionLayout->addWidget(m_quackHourly);
    m_quackNow = new QPushButton(m_host);
    m_quackNow->setObjectName(QString::fromUtf8("m_quackNow"));
    m_optionLayout->addWidget(m_quackNow);
    m_rightSideLayout->addLayout(m_optionLayout);

    verticalSpacer_2 = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    mainLayout->addItem(verticalSpacer_2);

    m_host->setWindowTitle(tr("Duck"));
    m_duckLabel->setText(QString());
    m_welcome->setText(tr("This is Mr Duck."));
    m_description->setText(tr("Mr. Duck shows us how to implement a Settings Applet in Arion."));
    m_quackMidnight->setText(tr("Quack at midnight"));
    m_quackHourly->setText(tr("Quack on the hour"));
    m_quackNow->setText(tr("Quack Now"));

    QMetaObject::connectSlotsByName(m_host);
}

