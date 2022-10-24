#include "colordialog.h"

ColorDialog::ColorDialog(QWidget *parent)
    :QDialog(parent),
     mainLayout(new QVBoxLayout(this)),
     layout_fg(new QFormLayout()),
     def_layout(new QHBoxLayout()),
     lbl_default(new QLabel(this)),
     m_defaultColor(new QToolButton(this)),
     layout_bg(new QFormLayout()),
     lbl_default_bg(new QLabel(this)),
     m_defaultBackground(new QToolButton(this)),
     line(new QFrame(this)),
     m_groupBox(new QGroupBox(this)),
     formLayout_2(new QFormLayout(m_groupBox)),
     labelStrings(new QLabel(m_groupBox)),
     layoutString(new QHBoxLayout()),
     m_colorStrings(new QToolButton(m_groupBox)),
     m_boldStrings(new QToolButton(m_groupBox)),
     m_italicString(new QToolButton(m_groupBox)),
     m_underlineString(new QToolButton(m_groupBox)),
     layoutComments(new QHBoxLayout()),
     labelComments(new QLabel(m_groupBox)),
     m_colorComments(new QToolButton(m_groupBox)),
     m_boldComments(new QToolButton(m_groupBox)),
     m_italicComments(new QToolButton(m_groupBox)),
     m_underlineComments(new QToolButton(m_groupBox)),
     lbl_keywords(new QLabel(m_groupBox)),
     layoutKeywords(new QHBoxLayout()),
     m_colorKeywords(new QToolButton(m_groupBox)),
     m_boldKeywords(new QToolButton(m_groupBox)),
     m_italicKeywords(new QToolButton(m_groupBox)),
     m_underlineKeywords(new QToolButton(m_groupBox)),
     lbl_classes_tags(new QLabel(m_groupBox)),
     layoutClasses(new QHBoxLayout()),
     m_colorClasses(new QToolButton(m_groupBox)),
     m_boldClasses(new QToolButton(m_groupBox)),
     m_italicClasses(new QToolButton(m_groupBox)),
     m_underlineClasses(new QToolButton(m_groupBox)),
     lbl_functions_parameters(new QLabel(m_groupBox)),
     layoutFunctions(new QHBoxLayout()),
     m_colorFunctions(new QToolButton(m_groupBox)),
     m_boldFunctions(new QToolButton(m_groupBox)),
     m_italicFunctions(new QToolButton(m_groupBox)),
     m_underlineFunctions(new QToolButton(m_groupBox)),
     m_buttons(new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this)),
     spacer(new QSpacerItem(1, 2, QSizePolicy::Minimum, QSizePolicy::Expanding))
{
    setObjectName("ColorPreferences");
    setWindowTitle(tr("Colors"));
#ifdef Q_WS_MAC
    setWindowFlag(Qt::Sheet, true);
#endif
#ifdef Q_WS_WIN
    setWindowIcon(QIcon());
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
#endif
    lbl_default->setText(tr("Default Text Color:"));
    lbl_default_bg->setText(tr("Default Background:"));

    m_defaultColor->setObjectName("ForegroundColor");
    m_defaultBackground->setObjectName("BackgroundColor");

    m_colorStrings->setObjectName("StringColor");
    m_colorComments->setObjectName("CommentColor");
    m_colorKeywords->setObjectName("KeywordColor");
    m_colorClasses->setObjectName("ClassColor");
    m_colorFunctions->setObjectName("FunctionColor");

    m_boldClasses->setObjectName("BoldClasses");
    m_italicClasses->setObjectName("ItalicClasses");
    m_underlineClasses->setObjectName("UnderlineClasses");
    m_boldComments->setObjectName("BoldComments");
    m_italicComments->setObjectName("ItalicComments");
    m_underlineComments->setObjectName("UnderlineComments");
    m_boldFunctions->setObjectName("BoldFunctions");
    m_italicFunctions->setObjectName("ItalicFunctions");
    m_underlineFunctions->setObjectName("UnderlineFunctions");
    m_boldKeywords->setObjectName("BoldKeywords");
    m_italicKeywords->setObjectName("ItalicKeywords");
    m_underlineKeywords->setObjectName("UnderlineKeywords");
    m_boldStrings->setObjectName("BoldStrings");
    m_italicString->setObjectName("ItalicStrings");
    m_underlineString->setObjectName("UnderlineStrings");

    layout_fg->setWidget(0, QFormLayout::LabelRole, lbl_default);
    layout_fg->setWidget(0, QFormLayout::FieldRole, m_defaultColor);
    def_layout->addLayout(layout_fg);

    layout_bg->setWidget(0, QFormLayout::LabelRole, lbl_default_bg);
    layout_bg->setWidget(0, QFormLayout::FieldRole, m_defaultBackground);
    def_layout->addLayout(layout_bg);
    mainLayout->addLayout(def_layout);

    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    layoutString->addWidget(m_colorStrings);
    layoutString->addWidget(m_boldStrings);
    layoutString->addWidget(m_italicString);
    layoutString->addWidget(m_underlineString);

    layoutComments->addWidget(m_colorComments);
    layoutComments->addWidget(m_boldComments);
    layoutComments->addWidget(m_italicComments);
    layoutComments->addWidget(m_underlineComments);

    layoutKeywords->addWidget(m_colorKeywords);
    layoutKeywords->addWidget(m_boldKeywords);
    layoutKeywords->addWidget(m_italicKeywords);
    layoutKeywords->addWidget(m_underlineKeywords);

    layoutClasses->addWidget(m_colorClasses);
    layoutClasses->addWidget(m_boldClasses);
    layoutClasses->addWidget(m_italicClasses);
    layoutClasses->addWidget(m_underlineClasses);

    layoutFunctions->addWidget(m_colorFunctions);
    layoutFunctions->addWidget(m_boldFunctions);
    layoutFunctions->addWidget(m_italicFunctions);
    layoutFunctions->addWidget(m_underlineFunctions);

    m_boldStrings->setIcon(QIcon(":/IconActions/Bold"));
    m_boldComments->setIcon(QIcon(":/IconActions/Bold"));
    m_boldKeywords->setIcon(QIcon(":/IconActions/Bold"));
    m_boldClasses->setIcon(QIcon(":/IconActions/Bold"));
    m_boldFunctions->setIcon(QIcon(":/IconActions/Bold"));

    m_italicString->setIcon(QIcon(":/IconActions/Italic"));
    m_italicComments->setIcon(QIcon(":/IconActions/Italic"));
    m_italicKeywords->setIcon(QIcon(":/IconActions/Italic"));
    m_italicClasses->setIcon(QIcon(":/IconActions/Italic"));
    m_italicFunctions->setIcon(QIcon(":/IconActions/Italic"));

    m_underlineString->setIcon(QIcon(":/IconActions/Underline"));
    m_underlineComments->setIcon(QIcon(":/IconActions/Underline"));
    m_underlineKeywords->setIcon(QIcon(":/IconActions/Underline"));
    m_underlineClasses->setIcon(QIcon(":/IconActions/Underline"));
    m_underlineFunctions->setIcon(QIcon(":/IconActions/Underline"));

    m_boldStrings->setCheckable(true);
    m_italicString->setCheckable(true);
    m_underlineString->setCheckable(true);

    m_boldComments->setCheckable(true);
    m_italicComments->setCheckable(true);
    m_underlineComments->setCheckable(true);

    m_boldKeywords->setCheckable(true);
    m_italicKeywords->setCheckable(true);
    m_underlineKeywords->setCheckable(true);

    m_boldClasses->setCheckable(true);
    m_italicClasses->setCheckable(true);
    m_underlineClasses->setCheckable(true);

    m_boldFunctions->setCheckable(true);
    m_italicFunctions->setCheckable(true);
    m_underlineFunctions->setCheckable(true);

    formLayout_2->setWidget(0, QFormLayout::LabelRole, labelStrings);
    formLayout_2->setLayout(0, QFormLayout::FieldRole, layoutString);
    formLayout_2->setLayout(1, QFormLayout::FieldRole, layoutComments);
    formLayout_2->setWidget(1, QFormLayout::LabelRole, labelComments);
    formLayout_2->setWidget(2, QFormLayout::LabelRole, lbl_keywords);
    formLayout_2->setLayout(2, QFormLayout::FieldRole, layoutKeywords);
    formLayout_2->setWidget(3, QFormLayout::LabelRole, lbl_classes_tags);
    formLayout_2->setLayout(3, QFormLayout::FieldRole, layoutClasses);
    formLayout_2->setWidget(4, QFormLayout::LabelRole, lbl_functions_parameters);
    formLayout_2->setLayout(4, QFormLayout::FieldRole, layoutFunctions);

    mainLayout->addWidget(m_groupBox);
    mainLayout->addWidget(m_buttons);
    mainLayout->addItem(spacer);

    m_groupBox->setTitle(tr("Syntax Highlighting"));
    labelStrings->setText(tr("Strings:"));
    labelComments->setText(tr("Comments:"));
    lbl_keywords->setText(tr("Keywords:"));
    lbl_classes_tags->setText(tr("Classes & Tags:"));
    lbl_functions_parameters->setText(tr("Functions & Parameters:"));

    connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

    connect(m_defaultColor, SIGNAL(clicked(bool)), this, SLOT(configureColor()));
    connect(m_defaultBackground, SIGNAL(clicked(bool)), this, SLOT(configureColor()));

    connect(m_colorStrings, SIGNAL(clicked(bool)), this, SLOT(configureColor()));
    connect(m_colorComments, SIGNAL(clicked(bool)), this, SLOT(configureColor()));
    connect(m_colorKeywords, SIGNAL(clicked(bool)), this, SLOT(configureColor()));
    connect(m_colorClasses, SIGNAL(clicked(bool)), this, SLOT(configureColor()));
    connect(m_colorFunctions, SIGNAL(clicked(bool)), this, SLOT(configureColor()));

    connect(m_boldClasses, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_italicClasses, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_underlineClasses, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));

    connect(m_boldFunctions, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_italicFunctions, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_underlineFunctions, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));

    connect(m_boldComments, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_italicComments, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_underlineComments, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));

    connect(m_boldKeywords, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_italicKeywords, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_underlineKeywords, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));

    connect(m_boldStrings, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_italicString, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));
    connect(m_underlineString, SIGNAL(clicked(bool)), this, SLOT(configureFontVariable()));


    readSettings();
}

ColorDialog::~ColorDialog()
{
    //myApp->requestPreferencesUpdate();
}

void ColorDialog::accept()
{
    myApp->requestPreferencesUpdate();
    QDialog::accept();

}
void ColorDialog::configureColor()
{
    QToolButton *b = qobject_cast<QToolButton*>(sender());
    QColorDialog cd(this);
    cd.setCurrentColor(getCurrentColorForButton(b));
    cd.setWindowTitle(tr("Select Color"));
    if(cd.exec() != QDialog::Accepted)
        return;

    QColor c = cd.currentColor();
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");
    settings.setValue(b->objectName(), c.name());
    settings.endGroup();
    settings.endGroup();

    b->setIcon(getIconForColor(c));
}

void ColorDialog::readSettings()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");
    m_defaultColor->setIcon(getIconForColor(QColor(settings.value(
                                 m_defaultColor->objectName(), QColor(Qt::black).name()).toString())));
    m_defaultBackground->setIcon(getIconForColor(QColor(settings.value(
                                 m_defaultBackground->objectName(), QColor(Qt::white).name()).toString())));

    m_colorStrings->setIcon(getIconForColor(QColor(settings.value(
                                 m_colorStrings->objectName(), QColor(Qt::black).name()).toString())));
    m_colorComments->setIcon(getIconForColor(QColor(settings.value(
                                 m_colorComments->objectName(), QColor(Qt::black).name()).toString())));
    m_colorKeywords->setIcon(getIconForColor(QColor(settings.value(
                                 m_colorKeywords->objectName(), QColor(Qt::black).name()).toString())));
    m_colorClasses->setIcon(getIconForColor(QColor(settings.value(
                                 m_colorClasses->objectName(), QColor(Qt::black).name()).toString())));
    m_colorFunctions->setIcon(getIconForColor(QColor(settings.value(
                                 m_colorFunctions->objectName(), QColor(Qt::black).name()).toString())));

    m_boldClasses->setChecked(settings.value("BoldClasses", false).toBool());
    m_italicClasses->setChecked(settings.value("ItalicClasses", false).toBool());
    m_underlineClasses->setChecked(settings.value("UnderlineClasses", false).toBool());
    m_boldComments->setChecked(settings.value("BoldComments", false).toBool());
    m_italicComments->setChecked(settings.value("ItalicComments", false).toBool());
    m_underlineComments->setChecked(settings.value("UnderlineComments", false).toBool());

    m_boldFunctions->setChecked(settings.value("BoldFunctions", false).toBool());
    m_italicFunctions->setChecked(settings.value("ItalicFunctions", false).toBool());
    m_underlineFunctions->setChecked(settings.value("UnderlineFunctions", false).toBool());
    m_boldKeywords->setChecked(settings.value("BoldKeywords", false).toBool());
    m_italicKeywords->setChecked(settings.value("ItalicKeywords", false).toBool());
    m_underlineKeywords->setChecked(settings.value("UnderlineKeywords", false).toBool());
    m_boldStrings->setChecked(settings.value("BoldStrings", false).toBool());
    m_italicString->setChecked(settings.value("ItalicStrings", false).toBool());
    m_underlineString->setChecked(settings.value("UnderlineStrings", false).toBool());

    settings.endGroup();
    settings.endGroup();
}

void ColorDialog::configureFontVariable()
{
    QToolButton *b = qobject_cast<QToolButton*>(sender());
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");

    settings.setValue(b->objectName(), b->isChecked());
    settings.endGroup();
    settings.endGroup();
}

QColor ColorDialog::getCurrentColorForButton(QToolButton *btn)
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");
    QColor c(settings.value(btn->objectName()).toString());
    settings.endGroup();
    settings.endGroup();

    return c;
}

QIcon ColorDialog::getIconForColor(QColor c)
{
    QPixmap p(16,16);
    p.fill(c);
    QIcon i(p);

    return i;
}
