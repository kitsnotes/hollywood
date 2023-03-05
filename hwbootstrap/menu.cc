#include "menu.h"
#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QSvgRenderer>
#include <QWindow>

MenuDialog::MenuDialog(QWidget *parent)
    : QDialog(parent)
    , m_lightBg(new QPixmap(700,480))
    , m_darkBg(new QPixmap(700,480))
{
    setMaximumSize(700,480);
    setMinimumSize(700,480);
    setWindowTitle(tr("Hollywood Setup Assistant"));
#ifdef Q_OS_WIN
    setWindowIcon(QIcon(":/Logo"));
#endif
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
}

void MenuDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.drawPixmap(QPointF(0,0), *m_lightBg, QRectF(0,0,700,480));
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

    m_install_sxs = new QCommandLinkButton(this);
    m_replace = new QCommandLinkButton(this);
    m_vm = new QCommandLinkButton(this);

    fl_actions->addWidget(m_install_sxs);
    fl_actions->addWidget(m_replace);
    fl_actions->addWidget(m_vm);

    hl_boxspacer->addItem(hsbr);
    hl_boxspacer->addLayout(fl_actions);
    hl_boxspacer->addItem(hs);

    lbl_copyright = new QLabel(this);
    lbl_copyright->setTextFormat(Qt::RichText);
    lbl_copyright->setAlignment(Qt::AlignCenter);
    lbl_copyright->setWordWrap(true);

    vl_main->addLayout(hl_boxspacer);
    vl_main->addItem(vs);
    vl_main->addWidget(lbl_copyright);


    auto os = "my current OS";
#ifdef Q_OS_MAC
    os = "macOS";
    m_replace->setEnabled(false);

    auto font = m_install_sxs->font();
    font.setPointSize(font.pointSize()+1);
    m_install_sxs->setFont(font);
    m_replace->setFont(font);
    m_vm->setFont(font);
#endif
#ifdef Q_OS_WIN
    os = "Windows";
#endif

    QIcon ico(":/go-next");
    m_install_sxs->setMinimumWidth(450);
    m_replace->setMinimumWidth(450);
    m_vm->setMinimumWidth(450);

    m_install_sxs->setIcon(ico);
    m_replace->setIcon(ico);
    m_vm->setIcon(ico);

    auto sxs = QCoreApplication::translate("Menu", "Install Hollywood alongside %1");
    auto sxs_d = QCoreApplication::translate("Menu", "Resize your system disk, install Hollywood in a dual-boot configuration, and copy selected settings from %1.");
    auto replace = QCoreApplication::translate("Menu", "Replace %1 with Hollywood");
    m_install_sxs->setText(sxs.arg(os));
    m_install_sxs->setDescription(sxs_d.arg(os));
    m_replace->setText(replace.arg(os));
#ifdef Q_OS_MAC
    m_replace->setDescription(QCoreApplication::translate("Menu", "This option is unavailable as macOS is required for your system to function."));
#else
    auto replace_d = QCoreApplication::translate("Menu", "Replace %1 with Hollywood and copy selected settings and your personal data.");
    m_replace->setDescription(replace_d.arg(os));
#endif
    m_vm->setText(QCoreApplication::translate("Menu", "Install Hollywood in a VM"));
    m_vm->setDescription(QCoreApplication::translate("Menu", "This will help you create a VM and install Hollywood in a hypervisor."));

    hollywood_label->setText(QCoreApplication::translate("Menu", "Hollywood", nullptr));
    hw_train->setText(QCoreApplication::translate("Menu", "", nullptr));
    lbl_copyright->setText(QCoreApplication::translate("Menu", "&copy; 2023 Originull Software and Contributors to the Hollywood Operating System.<br>Hollywood is free software; you have rights available to you under license. ", nullptr));
}

