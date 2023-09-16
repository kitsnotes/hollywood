#include "defaults.h"
#include "mimetypemodel.h"
#include "desktopentry.h"
#include <QTimer>
#include <QHeaderView>

#include <mimeapps.h>

DefaultsApplet::DefaultsApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{

}

bool DefaultsApplet::init()
{
    setupWidget();
    return true;
}

bool DefaultsApplet::loadSettings() { return true; }

QString DefaultsApplet::id() const
{
    return QLatin1String("org.originull.defaults");
}

QString DefaultsApplet::name() const
{
    return tr("Defaults");
}

QString DefaultsApplet::description() const
{
    return tr("Set default programs for file types and URL schemes.");
}

QIcon DefaultsApplet::icon() const
{
    return QIcon::fromTheme("preferences-desktop-default-applications");
}

QWidget *DefaultsApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category DefaultsApplet::category() const
{
    return Personal;
}

QStringList DefaultsApplet::searchTokens() const
{
    return QStringList();
}

void DefaultsApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("DefaultsAppletHost"));

    m_mime = new LSMimeApplications(this);
    m_mime->cacheAllDesktops();
    m_mainlayout = new QVBoxLayout(m_host);
    m_layout = new QFormLayout(0);

    auto description = new QLabel(m_host);
    auto subdesc = new QLabel(m_host);
    description->setText(tr("Select your preferred web browser, e-mail client and terminal below"));
    subdesc->setText(tr("Loading file types.... please wait"));

    m_browser = new QComboBox(m_host);
    m_email = new QComboBox(m_host);
    m_terminal = new QComboBox(m_host);
    m_filter = new QLineEdit(m_host);
    m_mimes = new QTreeView(m_host);
    m_mimes->setMinimumHeight(250);
    m_mimes->setDisabled(true);
    m_mimes->setRootIsDecorated(false);
    m_filter->setDisabled(true);

    m_filter->setPlaceholderText(tr("Search for a file type or URL scheme..."));
    auto bl = new QLabel(m_host);
    auto email = new QLabel(m_host);
    auto terminal = new QLabel(m_host);
    bl->setText(tr("Web Browser:"));
    email->setText(tr("E-Mail Client:"));
    terminal->setText(tr("Terminal:"));

    auto line = new QFrame(m_host);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    m_layout->setWidget(0, QFormLayout::SpanningRole, description);
    m_layout->setWidget(1, QFormLayout::LabelRole, bl);
    m_layout->setWidget(1, QFormLayout::FieldRole, m_browser);
    m_layout->setWidget(2, QFormLayout::LabelRole, email);
    m_layout->setWidget(2, QFormLayout::FieldRole, m_email);
    m_layout->setWidget(3, QFormLayout::LabelRole, terminal);
    m_layout->setWidget(3, QFormLayout::FieldRole, m_terminal);
    m_layout->setWidget(4, QFormLayout::SpanningRole, line);
    m_layout->setWidget(5, QFormLayout::SpanningRole, subdesc);
    m_layout->setWidget(6, QFormLayout::SpanningRole, m_filter);
    m_layout->setWidget(7, QFormLayout::SpanningRole, m_mimes);

    m_mainlayout->addItem(new QSpacerItem(150,1,QSizePolicy::Fixed, QSizePolicy::Preferred));
    m_mainlayout->addLayout(m_layout);
    m_mainlayout->addItem(new QSpacerItem(150,1,QSizePolicy::Fixed, QSizePolicy::Preferred));
    // populate our terminal options
    for(auto app : m_mime->categoryApps(QLatin1String("TerminalEmulator")))
    {
        if(app)
            m_terminal->addItem(app->icon(), app->value("Name").toString());
    }

    QTimer::singleShot(1, this, [this, subdesc] () {
        m_mime->processGlobalMimeCache();
        m_model = new MimeTypeModel(m_mime, this);
        m_proxy = new QSortFilterProxyModel(this);
        m_proxy->setSourceModel(m_model);
        m_mimes->setModel(m_proxy);
        m_mimes->setEnabled(true);
        m_filter->setEnabled(true);
        m_mimes->setSortingEnabled(true);
        m_proxy->sort(0, Qt::AscendingOrder);
        m_mimes->header()->setMinimumSectionSize(150);
        m_mimes->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_mimes->header()->setSectionResizeMode(1, QHeaderView::Interactive);

        subdesc->setText(tr("You can also adjust default applications for specific file types and URL schemes below"));
    });
}

