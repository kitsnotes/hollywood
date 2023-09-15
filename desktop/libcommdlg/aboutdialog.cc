#include "aboutdialog_p.h"
#include "aboutdialog.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QIcon>

static QByteArray copyBuildYear = __DATE__ + 7;

AboutDialogPrivate::AboutDialogPrivate(HWAboutDialog *parent)
    : d(parent)
    , layout(new QVBoxLayout(d))
    , m_iconLabel(new QLabel(d))
    , m_nameAboutLayout(new QVBoxLayout(0))
    , m_appName(new QLabel(d))
    , m_versionLabel(new QLabel(d))
    , m_infoLabel(new QLabel(d))
    , m_copyrightLabel(new QLabel(d))
{
    m_iconLabel->setPixmap(qApp->windowIcon().pixmap(QSize(128,128)));
    sp_Info = new QSpacerItem(0, 40, QSizePolicy::Minimum,
                              QSizePolicy::Fixed);
    sp_Copyright = new QSpacerItem(0, 40, QSizePolicy::Minimum,
                                   QSizePolicy::MinimumExpanding);

    layout->setObjectName("AboutDialogMainLayout");
    m_iconLabel->setObjectName(QString::fromUtf8("m_iconLabel"));
    m_nameAboutLayout->setObjectName("AboutDialogNameLayout");
    m_appName->setObjectName("AboutDialogAppNameLabel");
    m_versionLabel->setObjectName("AboutDialogVersionLabel");
    m_infoLabel->setObjectName("AboutDialogInfoLabel");
    m_copyrightLabel->setObjectName("AboutDialogCopyrightLabel");

    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_appName->setAlignment(Qt::AlignCenter);
    m_versionLabel->setAlignment(Qt::AlignCenter);
    m_infoLabel->setAlignment(Qt::AlignCenter);
    m_copyrightLabel->setAlignment(Qt::AlignCenter);

    QFont font = m_appName->font();
    font.setBold(true);
    font.setPointSize(font.pointSize()+2);
    m_appName->setFont(font);

    m_nameAboutLayout->addWidget(m_appName);
    m_nameAboutLayout->addWidget(m_versionLabel);

    layout->addWidget(m_iconLabel);
    layout->addLayout(m_nameAboutLayout);
    layout->addItem(sp_Info);
    layout->addWidget(m_infoLabel);
    layout->addItem(sp_Copyright);
    layout->addWidget(m_copyrightLabel);
}


HWAboutDialog::HWAboutDialog(QWidget *parent)
    :QDialog(parent)
    ,q(new AboutDialogPrivate(this))
{
    setMinimumWidth(250);
    setMaximumWidth(252);
    setMinimumHeight(320);
    setWindowFlag(Qt::Dialog, true);
    setWindowFlag(Qt::WindowMinimizeButtonHint, false);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    setAppName(qApp->applicationName());
    setAppDescription("");
    setAppVersion(tr("Version %1").arg(qApp->applicationVersion()));
    setAppCopyright(tr("Copyright © %1 %2").arg(QString(copyBuildYear)).arg(qApp->organizationName()));
    setMaximumSize(minimumSize().grownBy(QMargins(1,1,1,1)));
}

QString HWAboutDialog::appName() const
{
    return q->m_name;
}

void HWAboutDialog::setAppName(const QString &name)
{
    q->m_name = name;
    q->m_appName->setText(q->m_name);
    setWindowTitle(tr("About %1").arg(q->m_name));
    setFixedSize(minimumSize());
    emit appNameChanged(q->m_name);
}

QString HWAboutDialog::appVersion() const
{
    return q->m_version;
}

void HWAboutDialog::setAppVersion(const QString &version)
{
    q->m_version = version;
    q->m_versionLabel->setText(q->m_version);
    //setMaximumSize(minimumSize());
    //setFixedSize(minimumSize());
    emit appVersionChanged(q->m_version);
}

QString HWAboutDialog::appDescription() const
{
    return q->m_desc;
}

void HWAboutDialog::setAppDescription(const QString &description)
{
    q->m_desc = description;
    if(q->m_desc.isEmpty())
    {
        q->sp_Info->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
        q->m_infoLabel->setVisible(false);
    }
    else
    {
        q->sp_Info->changeSize(0, 15, QSizePolicy::Fixed, QSizePolicy::Fixed);
        q->m_infoLabel->setText(q->m_desc);
        q->m_infoLabel->setVisible(true);
    }
    //setMaximumSize(minimumSize());
    //setFixedSize(minimumSize());
    emit appDescriptionChanged(q->m_desc);
}

QString HWAboutDialog::appCopyright() const
{
    return q->m_copyright;
}

void HWAboutDialog::setAppCopyright(const QString &copyright)
{
    q->m_copyright = copyright;
    q->m_copyrightLabel->setText(q->m_copyright);
    //setMaximumSize(minimumSize());
    //setFixedSize(minimumSize());
    emit appCopyrightChanged(q->m_copyright);
}
