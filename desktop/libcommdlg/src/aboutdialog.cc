#include "aboutdialog.h"

static QByteArray copyBuildYear = __DATE__ + 7;

ADAboutDialog::ADAboutDialog(QWidget *parent)
    :QDialog(parent),
     layout(new QVBoxLayout(this)),
     m_iconLabel(new QLabel(this)),
     m_nameAboutLayout(new QVBoxLayout(0)),
     m_appName(new QLabel(this)),
     m_versionLabel(new QLabel(this)),
     m_infoLabel(new QLabel(this)),
     m_copyrightLabel(new QLabel(this))
{
    setupDialog();
    setMinimumWidth(250);
    setMaximumWidth(250);
    setWindowFlag(Qt::Dialog, true);
    setWindowFlag(Qt::WindowMinimizeButtonHint, false);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_iconLabel->setPixmap(qApp->windowIcon().pixmap(QSize(128,128)));

    setAppName(qApp->applicationName());
    setAppDescription("");
    setAppVersion(tr("Version %1").arg(qApp->applicationVersion()));
    setAppCopyright(tr("Copyright © %1 %2").arg(QString(copyBuildYear)).arg(qApp->organizationName()));
    //setMaximumSize(minimumSize());
    setFixedSize(minimumSize());
}

void ADAboutDialog::setupDialog()
{
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

QString ADAboutDialog::appName() const
{
    return m_name;
}

void ADAboutDialog::setAppName(const QString &name)
{
    m_name = name;
    m_appName->setText(m_name);
    setWindowTitle(tr("About %1").arg(m_name));
    //setMaximumSize(minimumSize().grownBy(QMargins(1,1,1,1)));
    setFixedSize(minimumSize());
    emit appNameChanged(m_name);
}

QString ADAboutDialog::appVersion() const
{
    return m_version;
}

void ADAboutDialog::setAppVersion(const QString &version)
{
    m_version = version;
    m_versionLabel->setText(m_version);
    //setMaximumSize(minimumSize());
    //setFixedSize(minimumSize());
    emit appVersionChanged(m_version);
}

QString ADAboutDialog::appDescription() const
{
    return m_desc;
}

void ADAboutDialog::setAppDescription(const QString &description)
{
    m_desc = description;
    if(m_desc.isEmpty())
    {
        sp_Info->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_infoLabel->setVisible(false);
    }
    else
    {
        sp_Info->changeSize(0, 15, QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_infoLabel->setText(m_desc);
        m_infoLabel->setVisible(true);
    }
    //setMaximumSize(minimumSize());
    //setFixedSize(minimumSize());
    emit appDescriptionChanged(m_desc);
}

QString ADAboutDialog::appCopyright() const
{
    return m_copyright;
}

void ADAboutDialog::setAppCopyright(const QString &copyright)
{
    m_copyright = copyright;
    m_copyrightLabel->setText(m_copyright);
    //setMaximumSize(minimumSize());
    //setFixedSize(minimumSize());
    emit appCopyrightChanged(m_copyright);
}
