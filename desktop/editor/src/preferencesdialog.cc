#include "preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
    :QDialog(parent),
     verticalLayout(new QVBoxLayout(this)),
     formLayout(new QFormLayout),
     lbl_font(new QLabel(this)),
     fontLayout(new QHBoxLayout),
     m_fontPreview(new QLineEdit(this)),
     m_changeFont(new QPushButton(this)),
     m_showLineNumbers(new QCheckBox(this)),
     m_highlightLine(new QCheckBox(this)),
     m_visualize_white_space(new QCheckBox(this)),
     lbl_caretSize(new QLabel(this)),
     m_caretWidth(new QSlider(this)),
     line(new QFrame(this)),
     lbl_tab_size(new QLabel(this)),
     m_tabsize(new QSpinBox(this)),
     m_spacesTabs(new QCheckBox(this)),
     line1(new QFrame(this)),
     lbl_smart(new QLabel(this)),
     m_auto_indent(new QCheckBox(this)),
     m_wrap_selection(new QCheckBox(this)),
     line2(new QFrame(this)),
     lbl_spelling(new QLabel(this)),
     cb_spelling(new QComboBox(this)),
     line3(new QFrame(this)),
     lbl_encoding(new QLabel(this)),
     cb_encoding(new QComboBox(this)),
     lbl_line_endings(new QLabel(this)),
     vl_line_endings(new QVBoxLayout),
     rb_unix(new QRadioButton(this)),
     rb_mac(new QRadioButton(this)),
     rb_windows(new QRadioButton(this)),
     m_spacer(new QSpacerItem(2, 2, QSizePolicy::Minimum, QSizePolicy::Expanding)),
     m_buttonbox(new QDialogButtonBox(this)),
     bg_line_endings(new QButtonGroup(this))
{
    setWindowTitle(tr("Preferences"));
    setWindowIcon(QIcon());
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    resize(450, 450);
    setMaximumWidth(500);

    lbl_font->setText(tr("Default Font:"));
    m_changeFont->setText(tr("Change..."));
    m_showLineNumbers->setText(tr("Show line numbers"));
    m_highlightLine->setText(tr("Highlight active line"));
    lbl_caretSize->setText(tr("Caret width:"));
    lbl_tab_size->setText(tr("Tab size:"));
    m_spacesTabs->setText(tr("Use spaces instead of tabs."));
    lbl_encoding->setText(tr("Default encoding:"));
    lbl_line_endings->setText(tr("Default line endings:"));
    rb_unix->setText(tr("Unix (\\n)"));
    rb_mac->setText(tr("Classic Macintosh (\\r)"));
    rb_windows->setText(tr("Windows (\\r\\n)"));
    lbl_smart->setText(tr("Smart editing features:"));
    m_auto_indent->setText(tr("Automatic indentation"));
    m_wrap_selection->setText(tr("Wrap selection with quotes or braces"));
    m_visualize_white_space->setText(tr("Visualize White Space"));
    // font display
    fontLayout->setSpacing(0);
    fontLayout->addWidget(m_fontPreview);
    fontLayout->addWidget(m_changeFont);
    m_fontPreview->setReadOnly(true);
    m_fontPreview->setAlignment(Qt::AlignCenter);

    m_caretWidth->setMinimum(1);
    m_caretWidth->setMaximum(4);
    m_caretWidth->setPageStep(1);
    m_caretWidth->setValue(1);
    m_caretWidth->setOrientation(Qt::Horizontal);
    m_caretWidth->setInvertedAppearance(false);
    m_caretWidth->setInvertedControls(true);
#ifdef Q_WS_MAC
    m_caretWidth->setTickPosition(QSlider::NoTicks);
#endif

    lbl_spelling->setText(tr("Spelling Dictionary:"));

    // decroative lines
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);

    m_tabsize->setMinimum(1);
    m_tabsize->setMaximum(30);
    m_tabsize->setValue(4);


    // follow the order of the struct (luvedit.h)
    bg_line_endings->addButton(rb_unix);
    bg_line_endings->setId(rb_unix, 0);
    bg_line_endings->addButton(rb_windows);
    bg_line_endings->setId(rb_windows, 1);
    bg_line_endings->addButton(rb_mac);
    bg_line_endings->setId(rb_mac, 2);

    vl_line_endings->addWidget(rb_unix);
    vl_line_endings->addWidget(rb_windows);
    vl_line_endings->addWidget(rb_mac);


    m_buttonbox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    formLayout->setWidget(0, QFormLayout::LabelRole, lbl_font);
    formLayout->setLayout(0, QFormLayout::FieldRole, fontLayout);
    formLayout->setWidget(1, QFormLayout::FieldRole, m_showLineNumbers);
    formLayout->setWidget(2, QFormLayout::FieldRole, m_highlightLine);
    formLayout->setWidget(3, QFormLayout::FieldRole, m_visualize_white_space);
    formLayout->setWidget(4, QFormLayout::LabelRole, lbl_caretSize);
    formLayout->setWidget(4, QFormLayout::FieldRole, m_caretWidth);
    formLayout->setWidget(5, QFormLayout::SpanningRole, line);
    formLayout->setWidget(6, QFormLayout::LabelRole, lbl_tab_size);
    formLayout->setWidget(6, QFormLayout::FieldRole, m_tabsize);
    formLayout->setWidget(7, QFormLayout::FieldRole, m_spacesTabs);
    formLayout->setWidget(8, QFormLayout::SpanningRole, line1);
    formLayout->setWidget(9, QFormLayout::LabelRole, lbl_smart);
    formLayout->setWidget(9, QFormLayout::FieldRole, m_auto_indent);
    formLayout->setWidget(10, QFormLayout::FieldRole, m_wrap_selection);
    formLayout->setWidget(11, QFormLayout::SpanningRole, line2);
    formLayout->setWidget(12, QFormLayout::LabelRole, lbl_spelling);
    formLayout->setWidget(12, QFormLayout::FieldRole, cb_spelling);

    formLayout->setWidget(13, QFormLayout::SpanningRole, line3);
    formLayout->setWidget(14, QFormLayout::LabelRole, lbl_encoding);
    formLayout->setWidget(14, QFormLayout::FieldRole, cb_encoding);
    formLayout->setWidget(15, QFormLayout::LabelRole, lbl_line_endings);
    formLayout->setLayout(15, QFormLayout::FieldRole, vl_line_endings);

    verticalLayout->addLayout(formLayout);
    verticalLayout->addItem(m_spacer);
    verticalLayout->addWidget(m_buttonbox);

    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(m_buttonbox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_buttonbox, SIGNAL(accepted()), this, SLOT(accept()));

    loadSettings();
}

void PreferencesDialog::loadSettings()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    uint le_val = settings.value("DefaultLineEnding", 0).toUInt();
    LineEnding line_ending;
    switch(le_val)
    {
    case 1:
        line_ending = Windows;
        rb_windows->setChecked(true);
        break;
    case 2:
        line_ending = Mac;
        rb_mac->setChecked(true);
        break;
    case 0:
    default:
        line_ending = Unix;
        rb_unix->setChecked(true);
    }

    uint pointSize = settings.value("DefaultFontSize").toUInt();
    QFont font;
    font.setFamily(settings.value("DefaultFontFamily").toString());
    //font.setPointSize(settings.value("DefaultFontSize").toUInt());
    QFontInfo fi(font);

    m_fontPreview->setText(QString("%1 %2pt").arg(fi.family()).arg(pointSize));
    m_fontPreview->setFont(font);
    m_fontPreview->setContextMenuPolicy(Qt::NoContextMenu);

    m_showLineNumbers->setChecked(settings.value("ShowLineNumbers", true).toBool());
    m_highlightLine->setChecked(settings.value("HighlightActiveLine", true).toBool());
    m_visualize_white_space->setChecked(settings.value("VisualizeWhiteSpace", false).toBool());
    unsigned int caretWidth = settings.value("CaretWidth", 1).toUInt();
    if(caretWidth > 4)
        caretWidth = 4;

    if(caretWidth < 1)
        caretWidth = 1;

    m_caretWidth->setValue(caretWidth);

    unsigned int tabWidth = settings.value("TabWidth", 4).toUInt();
    if(tabWidth > 30)
        tabWidth = 30;

    if(tabWidth < 1)
        tabWidth = 1;

    m_tabsize->setValue(tabWidth);
    m_spacesTabs->setChecked(settings.value("SpacesForTabs", false).toBool());

    m_auto_indent->setChecked(settings.value("AutoIndent", true).toBool());
    m_wrap_selection->setChecked(settings.value("WrapSelection", true).toBool());

    QStringList dictionaries = SpellCheck::availableDictionaries();
    QString prefDict = settings.value("SpellingDictionary", LUVEDIT_DEFAULT_DICT).toString();

    cb_spelling->addItems(dictionaries);
    cb_spelling->setCurrentText(prefDict);
}

void PreferencesDialog::saveSettings()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    settings.setValue("DefaultLineEnding", bg_line_endings->checkedId());
    settings.setValue("ShowLineNumbers", m_showLineNumbers->isChecked());
    settings.setValue("HighlightActiveLine", m_highlightLine->isChecked());
    settings.setValue("CaretWidth", m_caretWidth->value());
    settings.setValue("TabWidth", m_tabsize->value());
    settings.setValue("SpacesForTabs", m_spacesTabs->isChecked());
    settings.setValue("AutoIndent", m_auto_indent->isChecked());
    settings.setValue("WrapSelection", m_wrap_selection->isChecked());
    settings.setValue("VisualizeWhiteSpace", m_visualize_white_space->isChecked());
    settings.setValue("SpellingDictionary", cb_spelling->currentText());
    myApp->requestPreferencesUpdate();
}
