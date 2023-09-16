#include "generalapplet.h"
#include <QSettings>
#include <hollywood/hollywood.h>

HWGeneralApplet::HWGeneralApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool HWGeneralApplet::init()
{
    setupWidget();
    loadSettings();
    return true;
}

bool HWGeneralApplet::loadSettings()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup(QLatin1String("General"));
    uint _app = settings.value(QLatin1String("ApperanceMode"), 0).toUInt();
    if(_app > 2)
        _app = 0;   // default to light mode on invalid settings
    switch((SettingApperance)_app)
    {
    case SetDark:
        m_dark->setChecked(true);
        break;
    case SetTwilight:
        m_twilight->setChecked(true);
        break;
    case SetLight:
    default:
        m_light->setChecked(true);
    }

    auto acstr = settings.value(QLatin1String("AccentColor"),
                                QLatin1String(HOLLYWOOD_ACCENT_BLUE)).toString();

    auto accent_color = QColor(acstr);
    if(!accent_color.isValid())
        accent_color = QColor(QLatin1String(HOLLYWOOD_ACCENT_BLUE));

    auto color_verify = accent_color.name().toLower();
    if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_BLUE))
        ac_blue->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_RED))
        ac_red->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_YELLOW))
        ac_yellow->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_ORANGE))
        ac_orange->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_PURPLE))
        ac_purple->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_PINK))
        ac_pink->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_GREEN))
        ac_green->setChecked(true);
    else if(color_verify == QLatin1String(HOLLYWOOD_ACCENT_GRAY))
        ac_gray->setChecked(true);
    else
        qDebug() << "GeneralApplet: unknown accent color";

    uint fontsize = settings.value(QLatin1String("FontSize"), 1).toUInt();
    if(fontsize > 3)
        fontsize = 1;   // default to normal font sizes

    m_fontSize->setValue(fontsize);

    m_currentFontSize = fontsize;

    auto fontstr = settings.value("DefaultFont").toString();
    auto monostr = settings.value("MonospaceFont").toString();
    QFont font(fontstr);
    if(font.family() == fontstr)
        m_def_font->setFont(font);

    QFont mono(monostr);
    if(mono.family() == monostr)
        m_def_font->setFont(mono);

    return true;
}

bool HWGeneralApplet::saveSettings()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup("General");
    // Set ApperanceMode
    if(m_light->isChecked())
        settings.setValue(QLatin1String("ApperanceMode"), SetLight);
    else if(m_dark->isChecked())
        settings.setValue(QLatin1String("ApperanceMode"), SetDark);
    else if(m_twilight->isChecked())
        settings.setValue(QLatin1String("ApperanceMode"), SetTwilight);
    else
        qDebug() << "GeneralApplet: saveSettings: no selected apperance";

    // Set Accent Color
    if(ac_blue->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_BLUE));
    else if(ac_red->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_RED));
    else if(ac_yellow->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_YELLOW));
    else if(ac_orange->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_ORANGE));
    else if(ac_purple->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_PURPLE));
    else if(ac_pink->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_PINK));
    else if(ac_green->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_GREEN));
    else if(ac_gray->isChecked())
        settings.setValue(QLatin1String("AccentColor"),
                          QLatin1String(HOLLYWOOD_ACCENT_GRAY));
    else
        qDebug() << "GeneralApplet: saveSettings: no selected accent color";

    settings.setValue("DefaultFont", m_def_font->font().family());
    settings.setValue("MonospaceFont", m_def_fixedsys->font().family());

    settings.endGroup();
    return true;
}

QString HWGeneralApplet::id() const
{
    return QLatin1String("org.originull.hwsettings.general");
}

QString HWGeneralApplet::name() const
{
    return tr("General");
}

QString HWGeneralApplet::description() const
{
    return tr("General Settings");
}

QIcon HWGeneralApplet::icon() const
{
    const QIcon mine(QIcon::fromTheme("preferences-desktop"));
    return mine;
}

QWidget *HWGeneralApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category HWGeneralApplet::category() const
{
    return Personal;
}

QStringList HWGeneralApplet::searchTokens() const
{
    QStringList tokens;

    return tokens;
}

void HWGeneralApplet::widgetUpdate()
{
    saveSettings();
}

void HWGeneralApplet::fontSliderValueChanged()
{
    if(m_currentFontSize == m_fontSize->value())
        m_fontApply->setEnabled(false);
    else
        m_fontApply->setEnabled(true);

    QFont font = m_fontPreview->font();
    switch(m_fontSize->value())
    {
    case 3:
        font.setPointSize(HOLLYWOOD_PT_XLARGE);
        break;
    case 2:
        font.setPointSize(HOLLYWOOD_PT_LARGE);
        break;
    case 0:
        font.setPointSize(HOLLYWOOD_PT_SMALL);
        break;
    case 1:
    default:
        font.setPointSize(HOLLYWOOD_PT_NORMAL);
        break;
    }

    m_fontPreview->setFont(font);
}

void HWGeneralApplet::fontSizeApplyClicked()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup("General");
    settings.setValue("FontSize", m_fontSize->value());
    m_currentFontSize = m_fontSize->value();
    m_fontApply->setEnabled(false);
}

void HWGeneralApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("GeneralAppletHost"));

    auto mainLayout = new QFormLayout(m_host);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    mainLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    auto apperance = new QLabel(m_host);
    auto hl_apperance = new QHBoxLayout();
    bg_apperance = new QButtonGroup(m_host);
    bg_accentcolor = new QButtonGroup(m_host);
    m_light = new QRadioButton(m_host);
    m_dark = new QRadioButton(m_host);
    m_twilight = new QRadioButton(m_host);
    auto hs_app = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto accentcolor = new QLabel(m_host);
    auto aclayout = new QHBoxLayout();
    ac_blue = new QRadioButton(m_host);
    ac_purple = new QRadioButton(m_host);
    ac_pink = new QRadioButton(m_host);
    ac_red = new QRadioButton(m_host);
    ac_orange = new QRadioButton(m_host);
    ac_yellow = new QRadioButton(m_host);
    ac_green = new QRadioButton(m_host);
    ac_gray = new QRadioButton(m_host);
    auto hs_accent = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_allowAppAccent = new QCheckBox(m_host);
    auto line = new QFrame(m_host);
    auto fontsize = new QLabel(m_host);
    auto hl_fontsize = new QHBoxLayout();
    auto l_smfonticon = new QLabel(m_host);
    auto l_lgfonticon = new QLabel(m_host);
    m_fontSize = new QSlider(m_host);
    m_fontApply = new QPushButton(m_host);
    auto hs_font = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_fontPreview = new QLabel(m_host);
    auto lbl_browser = new QLabel(m_host);
    m_def_font = new QFontComboBox(m_host);
    m_def_font->setFontFilters(QFontComboBox::ScalableFonts);
    auto lbl_email = new QLabel(m_host);
    m_def_fixedsys = new QFontComboBox(m_host);
    m_def_fixedsys->setFontFilters(QFontComboBox::MonospacedFonts);
    auto vs_main = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    bg_apperance->addButton(m_light);
    bg_apperance->addButton(m_dark);
    bg_apperance->addButton(m_twilight);
    hl_apperance->addWidget(m_light);
    hl_apperance->addWidget(m_dark);
    hl_apperance->addWidget(m_twilight);
    hl_apperance->addSpacerItem(hs_app);

    bg_accentcolor->addButton(ac_blue);
    bg_accentcolor->addButton(ac_purple);
    bg_accentcolor->addButton(ac_pink);
    bg_accentcolor->addButton(ac_red);
    bg_accentcolor->addButton(ac_orange);
    bg_accentcolor->addButton(ac_yellow);
    bg_accentcolor->addButton(ac_green);
    bg_accentcolor->addButton(ac_gray);

    aclayout->addWidget(ac_blue);
    aclayout->addWidget(ac_purple);
    aclayout->addWidget(ac_pink);
    aclayout->addWidget(ac_red);
    aclayout->addWidget(ac_orange);
    aclayout->addWidget(ac_yellow);
    aclayout->addWidget(ac_green);
    aclayout->addWidget(ac_gray);
    aclayout->addSpacerItem(hs_accent);

    QPalette pb = ac_blue->palette();
    pb.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_BLUE));
    pb.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_blue->setPalette(pb);

    QPalette ppur = ac_blue->palette();
    ppur.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_PURPLE));
    ppur.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_purple->setPalette(ppur);

    QPalette ppink = ac_pink->palette();
    ppink.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_PINK));
    ppink.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_pink->setPalette(ppink);

    QPalette pred = ac_red->palette();
    pred.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_RED));
    pred.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_red->setPalette(pred);

    QPalette pora = ac_orange->palette();
    pora.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_ORANGE));
    pora.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_orange->setPalette(pora);

    QPalette py = ac_yellow->palette();
    py.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_YELLOW));
    py.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_yellow->setPalette(py);

    QPalette pg = ac_green->palette();
    pg.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_GREEN));
    pg.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_green->setPalette(pg);

    QPalette pgr = ac_gray->palette();
    pgr.setColor(QPalette::All, QPalette::Base, QColor(HOLLYWOOD_ACCENT_GRAY));
    pgr.setColor(QPalette::All, QPalette::ButtonText, QColor(Qt::white));
    ac_gray->setPalette(pgr);


    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    m_fontSize->setOrientation(Qt::Horizontal);
    hl_fontsize->addWidget(l_smfonticon);
    hl_fontsize->addWidget(m_fontSize);
    hl_fontsize->addWidget(l_lgfonticon);
    hl_fontsize->addWidget(m_fontApply);
    hl_fontsize->addSpacerItem(hs_font);

    mainLayout->setWidget(0, QFormLayout::LabelRole, apperance);
    mainLayout->setLayout(0, QFormLayout::FieldRole, hl_apperance);

    mainLayout->setWidget(1, QFormLayout::LabelRole, accentcolor);
    mainLayout->setLayout(1, QFormLayout::FieldRole, aclayout);
    mainLayout->setWidget(2, QFormLayout::FieldRole, m_allowAppAccent);

    mainLayout->setWidget(3, QFormLayout::SpanningRole, line);

    mainLayout->setWidget(4, QFormLayout::LabelRole, fontsize);
    mainLayout->setLayout(4, QFormLayout::FieldRole, hl_fontsize);

    mainLayout->setWidget(5, QFormLayout::FieldRole, m_fontPreview);
    mainLayout->setWidget(6, QFormLayout::LabelRole, lbl_browser);
    mainLayout->setWidget(6, QFormLayout::FieldRole, m_def_font);
    mainLayout->setWidget(7, QFormLayout::LabelRole, lbl_email);
    mainLayout->setWidget(7, QFormLayout::FieldRole, m_def_fixedsys);


    // KEEP THIS LAST in the layout!
    mainLayout->setItem(8, QFormLayout::SpanningRole, vs_main);

    apperance->setText(tr("Apperance:"));
    m_light->setText(tr("Light"));
    m_dark->setText(tr("Dark"));
    m_twilight->setText(tr("Twilight"));
    accentcolor->setText(tr("Accent color:"));
    m_allowAppAccent->setText(tr("Allow applications to override accent color"));
    fontsize->setText(tr("Font size:"));
    l_smfonticon->setText(QString());
    l_lgfonticon->setText(QString());
    lbl_browser->setText(tr("Default font:"));
    lbl_email->setText(tr("Default monospace font:"));

    m_fontPreview->setText(tr("The quick brown fox jumped over the lazy dog."));
    m_fontSize->setTickInterval(1);
    m_fontSize->setRange(0,3);
    m_fontSize->setValue(2);
    m_fontSize->setMaximumWidth(180);

    m_fontApply->setText(tr("Apply Size"));

    m_def_font->setEditable(false);
    m_def_fixedsys->setEditable(false);

    connect(m_fontApply, &QPushButton::clicked, this, &HWGeneralApplet::fontSizeApplyClicked);
    connect(m_fontSize, &QSlider::valueChanged, this, &HWGeneralApplet::fontSliderValueChanged);

    for(auto btn : bg_apperance->buttons())
        connect(btn, &QAbstractButton::toggled, this, &HWGeneralApplet::widgetUpdate);

    for(auto b : bg_accentcolor->buttons())
        connect(b, &QAbstractButton::toggled, this, &HWGeneralApplet::widgetUpdate);

}

