#include "preferences.h"
#include "application.h"
#include "profiletab.h"

#include <QSettings>

GeneralTab::GeneralTab(QWidget *parent)
    :QWidget(parent),
     gridLayout(new QGridLayout(0)),
     m_profile(new QComboBox(this)),
     m_cwd(new QComboBox(this)),
     m_profiletab(new QComboBox(this)),
     m_cwdtab(new QComboBox(this)),
     m_historyunlimited(new QRadioButton(this)),
     m_historyLimited(new QRadioButton(this)),
     m_historyLimit(new QSpinBox(this)),
     m_useCtrlTabs(new QCheckBox(this))
{
    auto vl_main = new QVBoxLayout(this);
    auto hl_sub = new QHBoxLayout();
    auto hs_left = new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hs_right = new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto l_newwnd = new QLabel(this);
    auto l_newtab = new QLabel(this);
    auto vl_newwnd = new QVBoxLayout();
    auto vl_newtab = new QVBoxLayout();
    auto l_buffersize = new QLabel(this);
    auto vl_history = new QVBoxLayout();
    auto hl_limit = new QHBoxLayout();
    auto vs_end = new QSpacerItem(20, 285, QSizePolicy::Minimum, QSizePolicy::Expanding);
    auto lines = new QLabel(this);
    hl_sub->addItem(hs_left);
    gridLayout->addWidget(l_newwnd, 0, 0, 1, 1);

    vl_newwnd->addWidget(m_profile);
    vl_newwnd->addWidget(m_cwd);
    vl_newtab->addWidget(m_profiletab);
    vl_newtab->addWidget(m_cwdtab);


    hl_limit->addWidget(m_historyLimited);
    hl_limit->addWidget(m_historyLimit);
    hl_limit->addWidget(lines);
    vl_history->addWidget(m_historyunlimited);
    vl_history->addLayout(hl_limit);

    gridLayout->addLayout(vl_newwnd, 0, 1, 1, 1);
    gridLayout->addWidget(l_newtab, 1, 0, 1, 1);
    gridLayout->addLayout(vl_newtab, 1, 1, 1, 1);
    gridLayout->addWidget(l_buffersize, 2, 0, 1, 1);
    gridLayout->addLayout(vl_history, 2, 1, 1, 1);
    gridLayout->addWidget(m_useCtrlTabs, 3, 1, 1, 1);
    hl_sub->addLayout(gridLayout);
    hl_sub->addItem(hs_right);
    hl_sub->setStretch(1, 1);
    vl_main->addLayout(hl_sub);

    vl_main->addItem(vs_end);

    l_newwnd->setText(tr("New windows open with:"));
    l_newtab->setText(tr("New tabs open with:"));
    l_buffersize->setText(tr("History buffer size:"));
    m_historyunlimited->setText(tr("Unlimited"));
    m_historyLimited->setText(tr("Limit to"));
    lines->setText(tr("lines"));
    m_useCtrlTabs->setText(tr("Use Shift+Ctrl+1 through Shit+Ctrl+9 to switch tabs"));

    m_profile->addItem(tr("Default Profile"), QLatin1String("Default"));
    m_profile->addItem(tr("Same Profile"), QLatin1String("Same"));
    m_profiletab->addItem(tr("Default Profile"), QLatin1String("Default"));
    m_profiletab->addItem(tr("Same Profile"), QLatin1String("Same"));

    m_cwd->addItem(tr("Default Working Directory"), QLatin1String("Default"));
    m_cwd->addItem(tr("Same Working Directory"), QLatin1String("Same"));
    m_cwdtab->addItem(tr("Default Working Directory"), QLatin1String("Default"));
    m_cwdtab->addItem(tr("Same Working Directory"), QLatin1String("Same"));

    connect(m_profile, &QComboBox::currentIndexChanged, this, &GeneralTab::comboBoxChanged);
    connect(m_cwd, &QComboBox::currentIndexChanged, this, &GeneralTab::comboBoxChanged);
    connect(m_profiletab, &QComboBox::currentIndexChanged, this, &GeneralTab::comboBoxChanged);
    connect(m_cwdtab, &QComboBox::currentIndexChanged, this, &GeneralTab::comboBoxChanged);

    connect(m_historyunlimited, &QRadioButton::toggled, this, &GeneralTab::radioChanged);
    connect(m_historyLimited, &QRadioButton::toggled, this, &GeneralTab::radioChanged);

    loadSettings();
}

GeneralTab::~GeneralTab()
{
    saveSettings();
}

void GeneralTab::loadSettings()
{
    QSettings settings("originull", "terminull");
    auto newwnd = settings.value("NewWindowProfile", 0).toInt();
    if(newwnd < 0 || newwnd > 1)
        newwnd = 0;
    m_profile->setCurrentIndex(newwnd);

    auto newwndcmd = settings.value("NewWindowCWD", 0).toInt();
    if(newwndcmd < 0 || newwndcmd > 1)
        newwndcmd = 0;
    m_cwd->setCurrentIndex(newwndcmd);

    auto newtab = settings.value("NewTabProfile", 1).toInt();
    if(newtab < 0 || newtab > 1)
        newtab = 0;
    m_profiletab->setCurrentIndex(newtab);

    auto newtabcmd = settings.value("NewTabCWD", 1).toInt();
    if(newtabcmd < 0 || newtabcmd > 1)
        newtabcmd = 0;
    m_cwdtab->setCurrentIndex(newtabcmd);

    m_historyLimit->setMaximum(9999);
    m_historyLimit->setMinimum(100);
    m_historyLimit->setSingleStep(50);
    auto buffer = settings.value("UnlimitedBuffer", true).toBool();
    if(buffer)
    {
        m_historyunlimited->setChecked(true);
        m_historyLimit->setDisabled(true);
    }
    else
    {
        m_historyLimited->setChecked(true);
        m_historyLimit->setDisabled(false);
    }

    m_historyLimit->setValue(settings.value("BufferLimit", 1500).toInt());
    m_useCtrlTabs->setChecked(settings.value("UseTabHotkeys", true).toBool());
}

void GeneralTab::saveSettings()
{
    qDebug() << "GeneralTab::saveSettings";
    QSettings settings("originull", "terminull");
    settings.setValue("BufferLimit", m_historyLimit->value());
    settings.setValue("UnlimitedBuffer", m_historyunlimited->isChecked());
    settings.setValue("UseTabHotKeys", m_useCtrlTabs->isChecked());
    settings.setValue("NewWindowProfile", m_profile->currentIndex());
    settings.setValue("NewWindowCWD", m_cwd->currentIndex());
    settings.setValue("NewTabProfile", m_profiletab->currentIndex());
    settings.setValue("NewTabCWD", m_cwdtab->currentIndex());
}

void GeneralTab::comboBoxChanged(int index)
{
   QSettings settings("originull", "terminull");
    auto cb = qobject_cast<QComboBox*>(sender());
    Q_ASSERT(cb);

    if(cb == m_profile)
        settings.setValue("NewWindowProfile", index);

    if(cb == m_cwd)
        settings.setValue("NewWindowCWD", index);

    if(cb == m_profiletab)
        settings.setValue("NewTabProfile", index);

    if(cb == m_cwdtab)
        settings.setValue("NewTabCWD", index);

    // TODO: emit settingsChanged() from app
}

void GeneralTab::radioChanged(bool checked)
{
    Q_UNUSED(checked);
    auto rb = qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(rb);

    if(rb == m_historyLimited)
        m_historyLimit->setEnabled(true);
    else
        m_historyLimit->setEnabled(false);
}


PreferencesDialog::PreferencesDialog(QWidget *parent)
    :HWPreferenceDialog(parent),
     m_general(new GeneralTab(this)),
     m_profile(new ProfileTab(this))
{
    auto icon = QIcon::fromTheme("configure");
    addPage(m_general, icon, tr("General"));
    auto profile = QIcon::fromTheme("user");
    addPage(m_profile, profile, tr("Profiles"));
}

