#include "menu.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QSvgRenderer>
#include <QWindow>
#include <QFile>
#include <QMessageBox>
#include <QCloseEvent>

#include "bootstrap.h"

MenuDialog::MenuDialog(QWidget *parent)
    : QDialog(parent)
    , m_lightBg(new QPixmap(700,480))
    , m_darkBg(new QPixmap(700,480))
{
    setMaximumSize(700,480);
    setMinimumSize(700,480);
    setWindowTitle(tr("Hollywood Utilities"));
    QSvgRenderer renderer(this);
    QPainter pl(m_lightBg);
    QRect r(0,0,700,480);
    renderer.load(QString(":/BGLight"));
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    pl.fillRect(r, QColor::fromRgb(239,239,239));
    pl.setBackground(QColor::fromRgb(239,239,239));
    renderer.render(&pl, r);

    setupUi();
    QPixmap orl(":/Logo");
    orl = orl.scaledToWidth(60,Qt::SmoothTransformation);
    logo_icon->setPixmap(orl);

    QFont font = hw_train->font();
    font.setPixelSize(35);
    hw_train->setFont(font);
    font.setBold(true);
    hollywood_label->setFont(font);

    checkActions();
}

void MenuDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.drawPixmap(QPointF(0,0), *m_lightBg, QRectF(0,0,700,480));
}

void MenuDialog::closeEvent(QCloseEvent *event)
{
    QMessageBox cancel(QMessageBox::Question,
                       tr("Exit Hollywood Utilities"),
                       tr("Do you wish to exit Hollywood Utilites?"),
                       QMessageBox::Yes | QMessageBox::No,
                       this);
    cancel.setDefaultButton(QMessageBox::No);
    cancel.setInformativeText(tr("Your system will reboot."));
    if(cancel.exec() == QMessageBox::Yes) {
        qApp->exit(1);
    } else {
        event->ignore();
    }
}

void MenuDialog::setupUi()
{
    vl_main = new QVBoxLayout(this);
    hl_branding = new QHBoxLayout();
    logo_icon = new QLabel(this);
    hollywood_label = new QLabel(this);
    hw_train = new QLabel(this);
    auto hl_boxspacer = new QHBoxLayout();
    auto hs_logo = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vs_brand = new QSpacerItem(4, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(logo_icon->sizePolicy().hasHeightForWidth());
    logo_icon->setSizePolicy(sizePolicy);
    sizePolicy.setHeightForWidth(hollywood_label->sizePolicy().hasHeightForWidth());
    hollywood_label->setSizePolicy(sizePolicy);
    sizePolicy.setHeightForWidth(hw_train->sizePolicy().hasHeightForWidth());
    hw_train->setSizePolicy(sizePolicy);

    hl_branding->addWidget(logo_icon);
    hl_branding->addWidget(hollywood_label);
    hl_branding->addWidget(hw_train);
    hl_branding->addItem(hs_logo);

    vl_main->addLayout(hl_branding);
    vl_main->addItem(vs_brand);

    auto hsbr = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);
    auto hs = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto vs = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    fl_actions = new QVBoxLayout();

    m_rescue = new QCommandLinkButton(this);
    m_browser = new QCommandLinkButton(this);
    m_reinstall = new QCommandLinkButton(this);
    m_terminull = new QCommandLinkButton(this);

    fl_actions->addWidget(m_rescue);
    fl_actions->addWidget(m_reinstall);
    fl_actions->addWidget(m_terminull);
    fl_actions->addWidget(m_browser);

    hl_boxspacer->addItem(hsbr);
    hl_boxspacer->addLayout(fl_actions);
    hl_boxspacer->addItem(hs);

    vl_main->addLayout(hl_boxspacer);
    vl_main->addItem(vs);

    QSize icosize(48,48);
    m_rescue->setMinimumWidth(450);
    m_reinstall->setMinimumWidth(450);
    m_terminull->setMinimumWidth(450);
    m_browser->setMinimumWidth(450);

    m_rescue->setIcon(QIcon::fromTheme("system-help"));
    m_reinstall->setIcon(QIcon::fromTheme("system-software-install"));
    m_terminull->setIcon(QIcon::fromTheme("utilities-terminal"));
    m_browser->setIcon(QIcon::fromTheme("internet-web-browser"));

    m_rescue->setIconSize(icosize);
    m_reinstall->setIconSize(icosize);
    m_terminull->setIconSize(icosize);
    m_browser->setIconSize(icosize);

    m_rescue->setText(tr("Rescue System or Run Hardware Tests"));
    m_rescue->setDescription("Run hardware tests, or mount your local system to fix issues "
                             "that prevent normal boot, or recover your files.");
    m_reinstall->setText(tr("Re-Install Hollywood"));
    m_reinstall->setDescription(tr("Re-install the Hollywood OS on your system disk."));
    m_terminull->setText(tr("Open Terminull"));
    m_terminull->setDescription(tr("Open a terminal instance."));
    m_browser->setText(tr("Web Browser"));
    m_browser->setDescription(tr("Open a web browser for further research on the Internet."));

    hollywood_label->setText(QCoreApplication::translate("Menu", "Hollywood", nullptr));
    hw_train->setText(QCoreApplication::translate("Menu", "Utilities", nullptr));

    connect(m_rescue, &QCommandLinkButton::clicked, HWUtilities::instance(), &HWUtilities::launch);
    connect(m_reinstall, &QCommandLinkButton::clicked, HWUtilities::instance(), &HWUtilities::launch);
    connect(m_terminull, &QCommandLinkButton::clicked, HWUtilities::instance(), &HWUtilities::launch);
    connect(m_browser, &QCommandLinkButton::clicked, HWUtilities::instance(), &HWUtilities::launch);

}

void MenuDialog::checkActions()
{
    QFile rescue("/usr/bin/hwrescue");
    if(!rescue.exists())
    {
        m_rescue->setEnabled(false);
        m_rescue->setDescription(tr("This option is unavailable."));
    }

    QFile setup("/usr/bin/hollywood-setup");
    if(!setup.exists())
    {
        m_reinstall->setEnabled(false);
        m_reinstall->setDescription(tr("This option is unavailable."));
    }

    QFile qw("/usr/bin/quickweb");
    if(!qw.exists())
    {
        m_browser->setEnabled(false);
        m_browser->setDescription(tr("This option is unavailable."));
    }

    QFile term("/usr/bin/terminull");
    if(!term.exists())
    {
        m_terminull->setEnabled(false);
        m_terminull->setDescription(tr("This option is unavailable."));
    }
}
