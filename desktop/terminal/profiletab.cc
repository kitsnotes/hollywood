#include "profiletab.h"
#include "profile.h"
#include "application.h"

#include <QDebug>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QMenu>
#include <QAction>
#include <QFileSystemModel>
#include <QCompleter>
#include <QSettings>
#include <QMessageBox>

ProfileTab::ProfileTab(QWidget *parent)
    :QWidget(parent),
     m_colordialog(new QColorDialog(this)),
     m_fontdialog(new QFontDialog(this)),
     m_profileList(new QListView(this)),
     m_model(new QStandardItemModel(this)),
     m_add(new QToolButton(this)),
     m_remove(new QToolButton(this)),
     m_options(new QToolButton(this)),
     m_setDefault(new QToolButton(this)),
     m_tabs(new QTabWidget(this)),
     m_tabText(new QWidget()),
     vl_tabText(new QVBoxLayout(m_tabText)),
     gb_background(new QGroupBox(m_tabText)),
     m_bgcolor(new QToolButton(gb_background)),
     m_bgimage(new QComboBox(gb_background)),
     m_bgtrans(new QSlider(gb_background)),
     gb_font(new QGroupBox(m_tabText)),
     m_font(new QLabel(gb_font)),
     m_chgfont(new QPushButton(gb_font)),
     gb_text(new QGroupBox(m_tabText)),
     m_textcolor(new QToolButton(gb_text)),
     m_boldtextcolor(new QToolButton(gb_text)),
     m_selectioncolor(new QToolButton(gb_text)),
     m_bold(new QCheckBox(gb_text)),
     m_blink(new QCheckBox(gb_text)),
     m_ansicolor(new QCheckBox(gb_text)),
     gb_colors(new QGroupBox(m_tabText)),
     m_ansiBlack(new QToolButton(gb_colors)),
     m_ansiRed(new QToolButton(gb_colors)),
     m_ansiGreen(new QToolButton(gb_colors)),
     m_ansiYellow(new QToolButton(gb_colors)),
     m_ansiBlue(new QToolButton(gb_colors)),
     m_ansiMagenta(new QToolButton(gb_colors)),
     m_ansiCyan(new QToolButton(gb_colors)),
     m_ansiWhite(new QToolButton(gb_colors)),
     m_ansiBrightBlack(new QToolButton(gb_colors)),
     m_ansiBrightRed(new QToolButton(gb_colors)),
     m_ansiBrightGreen(new QToolButton(gb_colors)),
     m_ansiBrightYellow(new QToolButton(gb_colors)),
     m_ansiBrightBlue(new QToolButton(gb_colors)),
     m_ansiBrightMagenta(new QToolButton(gb_colors)),
     m_ansiBrightCyan(new QToolButton(gb_colors)),
     m_ansiBrightWhite(new QToolButton(gb_colors)),
     gb_cursor(new QGroupBox(m_tabText)),
     m_curblock(new QRadioButton(gb_cursor)),
     m_curunderline(new QRadioButton(gb_cursor)),
     m_curibeam(new QRadioButton(gb_cursor)),
     m_blinkCur(new QCheckBox(gb_cursor)),
     m_curColor(new QToolButton(gb_cursor)),
     m_tabShell(new QWidget()),
     gb_startup(new QGroupBox(m_tabShell)),
     m_defaultshell(new QRadioButton(gb_startup)),
     m_altshell(new QRadioButton(gb_startup)),
     m_command(new QRadioButton(gb_startup)),
     m_altShellSelection(new QComboBox(gb_startup)),
     m_commandText(new QLineEdit(gb_startup)),
     m_runInShell(new QCheckBox(gb_startup)),
     gb_exit(new QGroupBox(m_tabShell)),
     m_exitAction(new QComboBox(gb_exit)),
     gb_askclose(new QGroupBox(m_tabShell)),
     m_askAlways(new QRadioButton(gb_askclose)),
     m_askNever(new QRadioButton(gb_askclose)),
     m_askSelective(new QRadioButton(gb_askclose)),
     m_selectiveCloseOptions(new QListWidget(gb_askclose)),
     m_tabWindow(new QWidget()),
     gb_windowtitle(new QGroupBox(m_tabWindow)),
     m_wndtitle(new QLineEdit(gb_windowtitle)),
     m_wnddir(new QCheckBox(gb_windowtitle)),
     m_wndpath(new QCheckBox(gb_windowtitle)),
     m_wndprocess(new QCheckBox(gb_windowtitle)),
     m_wndargs(new QCheckBox(gb_windowtitle)),
     m_wndshellcmd(new QCheckBox(gb_windowtitle)),
     m_wndprofile(new QCheckBox(gb_windowtitle)),
     m_wndtty(new QCheckBox(gb_windowtitle)),
     m_wnddimensions(new QCheckBox(gb_windowtitle)),
     gb_tabtitle(new QGroupBox(m_tabWindow)),
     m_tabdir(new QCheckBox(gb_tabtitle)),
     m_tabpath(new QCheckBox(gb_tabtitle)),
     m_tabproc(new QCheckBox(gb_tabtitle)),
     m_tabargs(new QCheckBox(gb_tabtitle)),
     m_tabtty(new QCheckBox(gb_tabtitle)),
     m_showothers(new QCheckBox(gb_tabtitle)),
     m_showactivity(new QCheckBox(gb_tabtitle)),
     m_tabAdvanced(new QWidget()),
     gb_terminfo(new QGroupBox(m_tabAdvanced)),
     m_terminfo(new QComboBox(gb_terminfo)),
     gb_input(new QGroupBox(m_tabAdvanced)),
     m_pastecheck(new QCheckBox(gb_input)),
     m_scrollbottom(new QCheckBox(gb_input)),
     gb_bell(new QGroupBox(m_tabAdvanced)),
     m_noaudiobell(new QRadioButton(gb_bell)),
     m_defaultsoundbell(new QRadioButton(gb_bell)),
     m_customaudiobell(new QRadioButton(gb_bell)),
     m_custombell(new QComboBox(gb_bell)),
     m_visualbell(new QCheckBox(gb_bell)),
     m_vbellonlymute(new QCheckBox(gb_bell)),
     m_notification(new QCheckBox(gb_bell))
{
    auto hl_main = new QHBoxLayout(this);
    // setup the main layout
    setLayout(hl_main);
    hl_main->setSpacing(4);
    hl_main->setContentsMargins(5, 5, 5, 5);
    auto vl_profileList = new QVBoxLayout();
    hl_main->addLayout(vl_profileList);
    hl_main->addWidget(m_tabs);
    hl_main->setStretch(0, 1);

    // setup the left side profile list
    m_profileList->setMaximumSize(QSize(140, 99999));
    m_profileList->setModel(m_model);
    m_profileList->setAlternatingRowColors(true);
    m_profileList->setIconSize(QSize(48,48));
    vl_profileList->setSpacing(0);
    vl_profileList->addWidget(m_profileList);
    auto l_profileListActions = new QHBoxLayout();
    l_profileListActions->setSpacing(0);
    l_profileListActions->addWidget(m_add);
    l_profileListActions->addWidget(m_remove);
    l_profileListActions->addWidget(m_options);
    l_profileListActions->addWidget(m_setDefault);

    m_add->setCheckable(true);
    m_remove->setCheckable(false);

    connect(m_add, &QPushButton::pressed, this, &ProfileTab::addProfile);
    connect(m_remove, &QPushButton::pressed, this, &ProfileTab::removeProfile);
    connect(m_setDefault, &QPushButton::pressed, this, &ProfileTab::setDefaultProfile);

    connect(m_model, &QStandardItemModel::itemChanged, this, &ProfileTab::profileChanged);
    auto extramenu = new QMenu(this);
    auto duplicate = extramenu->addAction(tr("&Duplicate Profile"));
    duplicate->setEnabled(false);
    auto restore = extramenu->addAction(tr("&Restore Default Profiles"));
    restore->setEnabled(false);
    extramenu->addSeparator();
    auto import = extramenu->addAction(tr("&Import..."));
    import->setEnabled(false);
    auto exportProfile = extramenu->addAction(tr("&Export..."));
    exportProfile->setEnabled(false);

    m_options->setMenu(extramenu);
    m_options->setPopupMode(QToolButton::InstantPopup);
    m_options->setArrowType(Qt::NoArrow);

    auto hs = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    l_profileListActions->addItem(hs);
    l_profileListActions->setStretch(3, 1);
    vl_profileList->addLayout(l_profileListActions);

    auto addicon = QIcon::fromTheme("list-add");
    if(addicon.isNull())
        m_add->setText(QLatin1String("+"));
    else
        m_add->setIcon(QIcon::fromTheme("list-add"));

    auto rmicon = QIcon::fromTheme("list-remove");
    if(rmicon.isNull())
        m_remove->setText(QLatin1String("-"));
    else
        m_remove->setIcon(QIcon::fromTheme("list-remove"));

    m_options->setText(tr("..."));
    m_setDefault->setText(tr("Default"));

    // setup the tabs
    setupStyleTab();
    setupShellTab();
    setupWindowTab();
    setupAdvancedTab();

    loadProfiles();

    m_tabs->setCurrentIndex(0);
    connect(m_profileList->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &ProfileTab::profileModelRowChanged);

    m_colordialog->setOption(QColorDialog::NoButtons);
    m_fontdialog->setOptions(QFontDialog::NoButtons|QFontDialog::MonospacedFonts);
}

ProfileTab::~ProfileTab()
{
    for(auto p : TerminalApplication::instance()->profiles())
        p->saveProfileToSettings();
}

void ProfileTab::setupStyleTab()
{
    // Text tab - group box "Background"
    gb_background->setFlat(true);
    auto vl_background = new QVBoxLayout(gb_background);
    auto l_bgcolor = new QLabel(gb_background);
    auto l_bgtransparency = new QLabel(gb_background);
    auto l_bgimage = new QLabel(gb_background);
    auto spacer = new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    auto hl_bgcolorimg = new QHBoxLayout();
    auto hl_bgcolor = new QHBoxLayout();
    auto hl_bgimage = new QHBoxLayout();
    auto hl_bgtransparency = new QHBoxLayout();
    hl_bgcolor->setSpacing(2);
    hl_bgcolor->addWidget(m_bgcolor);
    hl_bgcolor->addWidget(l_bgcolor);
    hl_bgcolorimg->addLayout(hl_bgcolor);
    hl_bgcolorimg->addItem(spacer);
    hl_bgcolorimg->addLayout(hl_bgimage);
    hl_bgimage->addWidget(l_bgimage);
    hl_bgimage->addWidget(m_bgimage);
    m_bgtrans->setOrientation(Qt::Horizontal);
    hl_bgtransparency->addWidget(l_bgtransparency);
    hl_bgtransparency->addWidget(m_bgtrans);
    vl_background->addLayout(hl_bgcolorimg);
    vl_background->addLayout(hl_bgtransparency);
    gb_background->setTitle(tr("Background"));
    l_bgcolor->setText(tr("Color"));
    l_bgimage->setText(tr("Image:"));
    l_bgtransparency->setText(tr("Transparency:"));
    m_bgtrans->setRange(0,100);

    m_bgimage->addItem(tr("No background image"), "None");
    m_bgimage->addItem(tr("Select..."), "Select");


    // Text tab - group box "Font"
    gb_font->setFlat(true);
    auto vl_font = new QVBoxLayout(gb_font);
    auto hl_font = new QHBoxLayout();
    auto hs_font = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hl_font->addWidget(m_font);
    hl_font->addItem(hs_font);
    hl_font->addWidget(m_chgfont);
    vl_font->addLayout(hl_font);
    gb_font->setTitle(tr("Font"));
    m_font->setText(tr("Font"));
    m_chgfont->setText(tr("Change..."));
    m_fontdialog->setWindowTitle(tr("Select Console Font"));

    // Text tab - group box "Text"
    gb_text->setFlat(true);
    auto gl_gbText = new QGridLayout(gb_text);
    auto l_textColor = new QLabel(gb_text);
    auto l_selColor = new QLabel(gb_text);
    auto l_boldTextColor = new QLabel(gb_text);
    auto gl_textcolors = new QGridLayout();
    auto vl_textchecks = new QVBoxLayout();
    auto hs_text = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gl_textcolors->addWidget(l_textColor, 0, 1, 1, 1);
    gl_textcolors->addWidget(m_textcolor, 0, 0, 1, 1);
    gl_textcolors->addWidget(l_selColor, 2, 1, 1, 1);
    gl_textcolors->addWidget(l_boldTextColor, 1, 1, 1, 1);
    gl_textcolors->addWidget(m_boldtextcolor, 1, 0, 1, 1);
    gl_textcolors->addWidget(m_selectioncolor, 2, 0, 1, 1);
    gl_gbText->addLayout(gl_textcolors, 0, 2, 1, 1);

    vl_textchecks->addWidget(m_bold);
    vl_textchecks->addWidget(m_blink);
    vl_textchecks->addWidget(m_ansicolor);
    gl_gbText->addLayout(vl_textchecks, 0, 0, 1, 1);
    gl_gbText->addItem(hs_text, 0, 1, 1, 1);
    gb_text->setTitle(tr("Text"));
    l_textColor->setText(tr("Text"));
    l_selColor->setText(tr("Selection"));
    l_boldTextColor->setText(tr("Bold Text"));
    m_bold->setText(tr("Use bold fonts"));
    m_blink->setText(tr("Allow blinking text"));
    m_ansicolor->setText(tr("Display ANSI colors"));

    // "Text" Tab - Group Box "Colors"
    gb_colors->setFlat(true);
    auto vl_colors = new QVBoxLayout(gb_colors);
    auto l_colnormal = new QLabel(gb_colors);
    auto l_colbright = new QLabel(gb_colors);
    auto hl_ansi = new QHBoxLayout();
    auto hl_ansi_bright = new QHBoxLayout();

    hl_ansi->setSpacing(2);
    hl_ansi->addWidget(m_ansiBlack);
    hl_ansi->addWidget(m_ansiRed);
    hl_ansi->addWidget(m_ansiGreen);
    hl_ansi->addWidget(m_ansiYellow);
    hl_ansi->addWidget(m_ansiBlue);
    hl_ansi->addWidget(m_ansiMagenta);
    hl_ansi->addWidget(m_ansiCyan);
    hl_ansi->addWidget(m_ansiWhite);
    hl_ansi->addWidget(l_colnormal);

    hl_ansi_bright->setSpacing(2);
    hl_ansi_bright->addWidget(m_ansiBrightBlack);
    hl_ansi_bright->addWidget(m_ansiBrightRed);
    hl_ansi_bright->addWidget(m_ansiBrightGreen);
    hl_ansi_bright->addWidget(m_ansiBrightYellow);
    hl_ansi_bright->addWidget(m_ansiBrightBlue);
    hl_ansi_bright->addWidget(m_ansiBrightMagenta);
    hl_ansi_bright->addWidget(m_ansiBrightCyan);
    hl_ansi_bright->addWidget(m_ansiBrightWhite);
    hl_ansi_bright->addWidget(l_colbright);

    vl_colors->addLayout(hl_ansi);
    vl_colors->addLayout(hl_ansi_bright);
    gb_colors->setTitle(tr("Colors"));
    l_colnormal->setText(tr("Normal"));
    l_colbright->setText(tr("Bright"));

    // "Text" tab - Group Box "Cursor"
    gb_cursor->setFlat(true);
    auto hl_cursor = new QHBoxLayout(gb_cursor);
    auto vl_cursorChoices = new QVBoxLayout();
    auto vl_cursor = new QVBoxLayout();
    auto hl_curColor = new QHBoxLayout();
    auto l_curColor = new QLabel(gb_cursor);
    auto hs_cursor = new QSpacerItem(327, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vs_cursorcolor = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vl_cursorChoices->addWidget(m_curblock);
    vl_cursorChoices->addWidget(m_curunderline);
    vl_cursorChoices->addWidget(m_curibeam);
    vl_cursorChoices->addWidget(m_blinkCur);
    hl_cursor->addLayout(vl_cursorChoices);
    hl_cursor->addItem(hs_cursor);
    hl_curColor->setSpacing(2);
    hl_curColor->addWidget(m_curColor);
    hl_curColor->addWidget(l_curColor);
    vl_cursor->addLayout(hl_curColor);
    vl_cursor->addItem(vs_cursorcolor);
    hl_cursor->addLayout(vl_cursor);
    gb_cursor->setTitle(tr("Cursor"));
    m_curblock->setText(tr("Block"));
    m_curunderline->setText(tr("Underline"));
    m_curibeam->setText(tr("I-Beam"));
    m_blinkCur->setText(tr("Blink cursor"));
    l_curColor->setText(tr("Color"));

    // setup the tab "Text" settings
    vl_tabText->setSpacing(1);
    vl_tabText->addWidget(gb_background);
    vl_tabText->addWidget(gb_font);
    vl_tabText->addWidget(gb_text);
    vl_tabText->addWidget(gb_colors);
    vl_tabText->addWidget(gb_cursor);

    auto vs_end = new QSpacerItem(20, 38, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vl_tabText->addItem(vs_end);

    m_tabs->addTab(m_tabText, tr("Style"));

    connect(m_bgimage, &QComboBox::activated, this, &ProfileTab::backgroundSelectorChanged);

    connect(m_curblock, &QRadioButton::toggled, this, &ProfileTab::cursorRadioChanged);
    connect(m_curunderline, &QRadioButton::toggled, this, &ProfileTab::cursorRadioChanged);
    connect(m_curibeam, &QRadioButton::toggled, this, &ProfileTab::cursorRadioChanged);

    connect(m_bold, &QCheckBox::toggled, this,  &ProfileTab::styleCheckboxChanged);
    connect(m_blink, &QCheckBox::toggled, this,  &ProfileTab::styleCheckboxChanged);
    connect(m_ansicolor, &QCheckBox::toggled, this,  &ProfileTab::styleCheckboxChanged);
    connect(m_blinkCur, &QCheckBox::toggled, this,  &ProfileTab::styleCheckboxChanged);

    connect(m_bgcolor, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_textcolor, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_boldtextcolor, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_selectioncolor, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_curColor, &QToolButton::pressed, this, &ProfileTab::selectColor);

    connect(m_ansiBrightBlack, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightRed, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightGreen, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightYellow, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightBlue, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightMagenta, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightCyan, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBrightWhite, &QToolButton::pressed, this, &ProfileTab::selectColor);

    connect(m_ansiBlack, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiRed, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiGreen, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiYellow, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiBlue, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiMagenta, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiCyan, &QToolButton::pressed, this, &ProfileTab::selectColor);
    connect(m_ansiWhite, &QToolButton::pressed, this, &ProfileTab::selectColor);

    connect(m_chgfont, &QPushButton::pressed, this, &ProfileTab::showFontDialog);
    connect(m_bgtrans, &QSlider::valueChanged, this, &ProfileTab::transparencySliderChanged);

    connect(m_colordialog, &QColorDialog::currentColorChanged,
            this, &ProfileTab::colorDialogChanged);
    connect(m_colordialog, &QColorDialog::finished,
            this, &ProfileTab::colorDialogClosed);

    connect(m_fontdialog, &QFontDialog::currentFontChanged,
            this, &ProfileTab::fontChanged);
}

void ProfileTab::setupShellTab()
{
    gb_startup->setFlat(true);
    auto vl_command = new QVBoxLayout();
    vl_command->addWidget(m_commandText);
    vl_command->addWidget(m_runInShell);

    auto fl_startup = new QFormLayout(gb_startup);
    fl_startup->setContentsMargins(-1, 9, 9, 9);
    fl_startup->setWidget(0, QFormLayout::LabelRole, m_defaultshell);
    fl_startup->setWidget(1, QFormLayout::LabelRole, m_altshell);
    fl_startup->setWidget(1, QFormLayout::FieldRole, m_altShellSelection);
    fl_startup->setWidget(2, QFormLayout::LabelRole, m_command);
    fl_startup->setLayout(2, QFormLayout::FieldRole, vl_command);

    gb_startup->setTitle(tr("On startup:"));
    m_defaultshell->setText(tr("Start login shell"));
    m_altshell->setText(tr("Start alternative shell"));
    m_command->setText(tr("Run command"));
    m_runInShell->setText(tr("Run inside shell"));

    connect(m_defaultshell, &QRadioButton::toggled, this, &ProfileTab::startupRadioChanged);
    connect(m_altshell, &QRadioButton::toggled, this, &ProfileTab::startupRadioChanged);
    connect(m_command, &QRadioButton::toggled, this, &ProfileTab::startupRadioChanged);

    connect(m_altShellSelection, &QComboBox::currentIndexChanged, this, &ProfileTab::onAltShellIndexChanged);

    auto completer = new QCompleter(this);
    auto fsm = new QFileSystemModel(this);
    fsm->setRootPath("/");
    //fsm->setFilter(QDir::Executable);
    completer->setModel(fsm);
    m_commandText->setCompleter(completer);

    gb_exit->setFlat(true);
    gb_exit->setTitle(tr("On exit:"));
    auto exitLayout = new QHBoxLayout(gb_exit);
    auto exitSpacer = new QSpacerItem(368, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    exitLayout->addWidget(m_exitAction);
    exitLayout->addItem(exitSpacer);
    m_exitAction->addItem(tr("Close the tab"));
    m_exitAction->addItem(tr("Close the tab if the shell exited cleanly"));
    m_exitAction->addItem(tr("Don't close the tab"));
    connect(m_exitAction, &QComboBox::currentIndexChanged, this, &ProfileTab::onOnExitIndexChanged);

    // Ask Before Close group box
    gb_askclose->setFlat(true);
    auto askcloseLayout = new QVBoxLayout(gb_askclose);
    auto vl_selectiveCloseOptions = new QVBoxLayout();
    vl_selectiveCloseOptions->addWidget(m_selectiveCloseOptions);

    auto addButton = new QToolButton(gb_askclose);
    auto removeButton = new QToolButton(gb_askclose);
    auto buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0,0,0,0);

    auto addicon = QIcon::fromTheme("list-add");
    if(addicon.isNull())
        addButton->setText(QLatin1String("+"));
    else
        addButton->setIcon(QIcon::fromTheme("list-add"));

    auto rmicon = QIcon::fromTheme("list-remove");
    if(rmicon.isNull())
        removeButton->setText(QLatin1String("-"));
    else
        removeButton->setIcon(QIcon::fromTheme("list-remove"));

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addItem(buttonSpacer);
    vl_selectiveCloseOptions->addLayout(buttonLayout);

    askcloseLayout->addWidget(m_askAlways);
    askcloseLayout->addWidget(m_askNever);
    askcloseLayout->addWidget(m_askSelective);
    askcloseLayout->addLayout(vl_selectiveCloseOptions);
    gb_askclose->setTitle(tr("Ask before closing:"));
    m_askAlways->setText(tr("Always"));
    m_askNever->setText(tr("Never"));
    m_askSelective->setText(tr("Only if there are running processes other than the shell and:"));
    m_selectiveCloseOptions->setAlternatingRowColors(true);

    connect(m_askAlways, &QRadioButton::toggled, this, &ProfileTab::quitConsentRadioChanged);
    connect(m_askNever, &QRadioButton::toggled, this, &ProfileTab::quitConsentRadioChanged);
    connect(m_askSelective, &QRadioButton::toggled, this, &ProfileTab::quitConsentRadioChanged);

    auto endSpacer = new QSpacerItem(1, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);
    auto shellLayout = new QVBoxLayout(m_tabShell);
    shellLayout->addWidget(gb_startup);
    shellLayout->addWidget(gb_exit);
    shellLayout->addWidget(gb_askclose);
    shellLayout->addItem(endSpacer);

    listAltShells();

    m_tabs->addTab(m_tabShell, tr("Shell"));
}

void ProfileTab::setupWindowTab()
{
    auto endSpacer = new QSpacerItem(1, 1,
                QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_tabs->addTab(m_tabWindow, tr("Window"));
    auto vl_window = new QVBoxLayout(m_tabWindow);

    // BEGIN GROUPBOX: Window Title
    gb_windowtitle->setFlat(true);
    gb_windowtitle->setTitle(tr("Window title:"));
    auto vl_windowtitle = new QVBoxLayout(gb_windowtitle);
    auto hl_wndtitle_checks = new QHBoxLayout();
    auto vl_wt_chkleft = new QVBoxLayout();

    auto hl_wndpath = new QHBoxLayout();
    auto hl_wndargs = new QHBoxLayout();
    auto hs_chklayout_sep = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vl_wt_chkright = new QVBoxLayout();
    // TODO: Set to width of a check box (for proper indentation)
    auto spacer_wndpath = new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);
    auto spacer_wndargs = new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);

    hl_wndpath->addItem(spacer_wndpath);
    hl_wndpath->addWidget(m_wndpath);
    hl_wndargs->addItem(spacer_wndargs);
    hl_wndargs->addWidget(m_wndargs);
    m_wndpath->setDisabled(true);
    m_wndargs->setDisabled(true);

    connect(m_wnddir, &QCheckBox::toggled, m_wndpath, &QCheckBox::setEnabled);
    connect(m_wndprocess, &QCheckBox::toggled, m_wndargs, &QCheckBox::setEnabled);

    vl_wt_chkleft->addWidget(m_wnddir);
    vl_wt_chkleft->addLayout(hl_wndpath);
    vl_wt_chkleft->addWidget(m_wndprocess);
    vl_wt_chkleft->addLayout(hl_wndargs);

    vl_wt_chkright->addWidget(m_wndshellcmd);
    vl_wt_chkright->addWidget(m_wndprofile);
    vl_wt_chkright->addWidget(m_wndtty);
    vl_wt_chkright->addWidget(m_wnddimensions);

    m_wnddir->setText(tr("Working directory or document"));
    m_wndpath->setText(tr("Path"));
    m_wndprocess->setText(tr("Active process name"));
    m_wndargs->setText(tr("Arguments"));

    m_wndshellcmd->setText(tr("Shell command name"));
    m_wndprofile->setText(tr("Profile name"));
    m_wndtty->setText(tr("TTY name"));
    m_wnddimensions->setText(tr("Dimensions"));

    hl_wndtitle_checks->addLayout(vl_wt_chkleft);
    hl_wndtitle_checks->addItem(hs_chklayout_sep);
    hl_wndtitle_checks->addLayout(vl_wt_chkright);

    vl_windowtitle->addWidget(m_wndtitle);
    vl_windowtitle->addLayout(hl_wndtitle_checks);
    // END GROUPBOX: Window Title

    // BEGIN GROUPBOX: Tab Title
    gb_tabtitle->setFlat(true);
    gb_tabtitle->setTitle(tr("Tab title"));
    auto vl_tabtitle = new QVBoxLayout(gb_tabtitle);
    auto hl_tabtitleoptions = new QHBoxLayout();
    auto vl_tabchk_left = new QVBoxLayout();
    auto hl_tabpath = new QHBoxLayout();
    auto sp_tabtitleopts = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vl_tabchk_right = new QVBoxLayout();
    auto hl_tabargs = new QHBoxLayout();
    // TODO: Set to width of a check box (for proper indentation)
    auto sp_tabargs = new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);
    auto sp_tabpath = new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);

    hl_tabpath->addItem(sp_tabpath);
    hl_tabpath->addWidget(m_tabpath);

    vl_tabchk_left->addWidget(m_tabdir);
    vl_tabchk_left->addLayout(hl_tabpath);

    hl_tabargs->addItem(sp_tabargs);
    hl_tabargs->addWidget(m_tabargs);
    vl_tabchk_right->addWidget(m_tabproc);
    vl_tabchk_right->addLayout(hl_tabargs);

    hl_tabtitleoptions->addLayout(vl_tabchk_left);
    hl_tabtitleoptions->addItem(sp_tabtitleopts);
    hl_tabtitleoptions->addLayout(vl_tabchk_right);

    vl_tabtitle->addLayout(hl_tabtitleoptions);
    vl_tabtitle->addWidget(m_tabtty);
    vl_tabtitle->addWidget(m_showothers);
    vl_tabtitle->addWidget(m_showactivity);

    m_tabpath->setDisabled(true);
    m_tabargs->setDisabled(true);

    connect(m_tabdir, &QCheckBox::toggled, m_tabpath, &QCheckBox::setEnabled);
    connect(m_tabproc, &QCheckBox::toggled, m_tabargs, &QCheckBox::setEnabled);

    m_tabdir->setText(tr("Working directory or document"));
    m_tabpath->setText(tr("Path"));
    m_tabproc->setText(tr("Active process name"));
    m_tabargs->setText(tr("Arguments"));
    m_tabtty->setText(tr("TTY name"));

    m_showothers->setText(tr("Show selected items when there is a custom title"));
    m_showactivity->setText(tr("Show activity indicator"));

    // END GROUPBOX: Tab Title

    vl_window->addWidget(gb_windowtitle);
    vl_window->addWidget(gb_tabtitle);
    vl_window->addItem(endSpacer);
}

void ProfileTab::setupAdvancedTab()
{
    auto vl_main = new QVBoxLayout(m_tabAdvanced);

    gb_terminfo->setFlat(true);
    gb_terminfo->setTitle(tr("Terminfo"));
    auto vl_terminfo = new QVBoxLayout(gb_terminfo);
    auto hl_terminfo = new QHBoxLayout();
    auto lbl_terminfo = new QLabel(gb_terminfo);
    auto spc_ti = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lbl_terminfo->setText(tr("Declare terminal as:"));
    hl_terminfo->addWidget(lbl_terminfo);
    hl_terminfo->addWidget(m_terminfo);
    hl_terminfo->addItem(spc_ti);
    vl_terminfo->addLayout(hl_terminfo);

    // TODO: move this elsewhere? better terminfo suppot?
    m_terminfo->addItem(QLatin1String("xterm"));
    m_terminfo->addItem(QLatin1String("xterm-16color"));
    m_terminfo->addItem(QLatin1String("xterm-256color"));
    m_terminfo->setEditable(true);

    gb_input->setFlat(true);
    gb_input->setTitle(tr("Input"));
    auto vl_input = new QVBoxLayout(gb_input);
    vl_input->addWidget(m_pastecheck);
    m_pastecheck->setVisible(false);
    vl_input->addWidget(m_scrollbottom);
    m_scrollbottom->setText(tr("Scroll to bottom on input"));
    connect(m_scrollbottom, &QCheckBox::toggled, this, &ProfileTab::inputCheckChanged);
    connect(m_pastecheck, &QCheckBox::toggled, this, &ProfileTab::inputCheckChanged);

    gb_bell->setFlat(true);
    gb_bell->setTitle(tr("Bell"));
    auto vl_bell = new QVBoxLayout(gb_bell);
    auto hl_custombell = new QHBoxLayout();
    auto hl_onlymute = new QHBoxLayout();
    auto hs_custombell = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    // TODO: proper indent based on QStyle
    auto hs_onlymute = new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum);
    auto vs_end = new QSpacerItem(20, 303, QSizePolicy::Minimum, QSizePolicy::Expanding);

    hl_custombell->addWidget(m_customaudiobell);
    hl_custombell->addWidget(m_custombell);
    hl_custombell->addItem(hs_custombell);
    hl_onlymute->addItem(hs_onlymute);
    hl_onlymute->addWidget(m_vbellonlymute);

    vl_bell->addWidget(m_noaudiobell);
    vl_bell->addWidget(m_defaultsoundbell);
    vl_bell->addLayout(hl_custombell);
    vl_bell->addWidget(m_visualbell);
    vl_bell->addLayout(hl_onlymute);
    vl_bell->addWidget(m_notification);

    connect(m_noaudiobell, &QRadioButton::toggled, this, &ProfileTab::bellRadioChanged);
    connect(m_defaultsoundbell, &QRadioButton::toggled, this, &ProfileTab::bellRadioChanged);
    connect(m_customaudiobell, &QRadioButton::toggled, this, &ProfileTab::bellRadioChanged);

    m_noaudiobell->setText(tr("No audible bell"));
    m_defaultsoundbell->setText(tr("Audible bell with default notification sound"));
    m_customaudiobell->setText(tr("Audible bell with custom sound:"));
    m_custombell->setDisabled(true);

    m_visualbell->setText(tr("Visual bell"));
    m_vbellonlymute->setText(tr("Only when sound is muted"));
    m_notification->setText(tr("System notification"));

    m_vbellonlymute->setDisabled(true);
    connect(m_visualbell, &QCheckBox::toggled, m_vbellonlymute, &QCheckBox::setEnabled);

    vl_main->addWidget(gb_terminfo);
    vl_main->addWidget(gb_input);
    vl_main->addWidget(gb_bell);
    vl_main->addItem(vs_end);
    m_tabs->addTab(m_tabAdvanced, tr("Advanced"));
}

void ProfileTab::loadProfiles()
{
    m_model->clear();
    for(auto profile : TerminalApplication::instance()->profiles())
    {
        auto item = new QStandardItem(profile->previewIcon(), profile->profileName());
        item->setData(profile->profileName());
        m_model->appendRow(item);
    }
    if(m_model->rowCount() == 0)
        return;

    if(m_model->rowCount() == 1)
        m_remove->setDisabled(true);

    m_selectedProfile = TerminalApplication::instance()->profileByName(m_model->item(0)->text());
    m_profileList->selectionModel()->select(m_model->index(0,0), QItemSelectionModel::Select);
    loadProfileData(m_model->item(0)->text());
}

void ProfileTab::loadProfileData(const QString &profile)
{
    auto *p = TerminalApplication::instance()->profileByName(profile);
    m_selectedProfile = p;
    if(p == nullptr)
        return;

    m_font->setFont(p->font());
    auto fontstr = QString("%1 %2pt").arg(p->font().family()).arg(p->font().pointSize());
    m_font->setText(fontstr);

    m_bold->setChecked(p->useBold());
    m_blink->setChecked(p->textBlink());
    m_ansicolor->setChecked(p->useANSIColor());
    int trans = p->backgroundTransparency() * 100;
    if(p->backgroundTransparency() == 0.0)
        trans = 0;
    if(p->backgroundTransparency() == 1.0)
        trans = 100;

    if(p->backgroundImage().isEmpty())
        m_bgimage->setCurrentText(tr("No background image"));

    m_bgtrans->setValue(trans);
    m_bgcolor->setIcon(createColorIcon(p->backgroundColor()));
    m_textcolor->setIcon(createColorIcon(p->textColor()));
    m_boldtextcolor->setIcon(createColorIcon(p->boldColor()));
    m_selectioncolor->setIcon(createColorIcon(p->selectionColor()));

    m_ansiBlack->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Black)));
    m_ansiRed->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Red)));
    m_ansiGreen->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Green)));
    m_ansiYellow->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Yellow)));
    m_ansiBlue->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Blue)));
    m_ansiMagenta->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Magenta)));
    m_ansiCyan->setIcon(createColorIcon(p->ansiColor(TerminalProfile::Cyan)));
    m_ansiWhite->setIcon(createColorIcon(p->ansiColor(TerminalProfile::White)));

    m_ansiBrightBlack->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightBlack)));
    m_ansiBrightRed->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightRed)));
    m_ansiBrightGreen->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightGreen)));
    m_ansiBrightYellow->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightYellow)));
    m_ansiBrightBlue->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightBlue)));
    m_ansiBrightMagenta->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightMagenta)));
    m_ansiBrightCyan->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightCyan)));
    m_ansiBrightWhite->setIcon(createColorIcon(p->ansiColor(TerminalProfile::BrightWhite)));


    switch(p->cursorType())
    {
    case TerminalProfile::Block:
        m_curblock->setChecked(true);
        break;
    case TerminalProfile::Underline:
        m_curunderline->setChecked(true);
        break;
    case TerminalProfile::IBeam:
        m_curibeam->setChecked(true);
        break;
    default:
        break;
    }

    m_blinkCur->setChecked(p->cursorBlink());
    m_curColor->setIcon(createColorIcon(p->cursorColor()));

    switch(p->startupBehavior())
    {
    case TerminalProfile::LoginShell:
        m_defaultshell->setChecked(true);
        m_altShellSelection->setDisabled(true);
        m_commandText->setDisabled(true);
        m_runInShell->setDisabled(true);
        break;
    case TerminalProfile::CustomShell:
        m_altshell->setChecked(true);
        m_altShellSelection->setDisabled(false);
        m_commandText->setDisabled(true);
        m_runInShell->setDisabled(true);
        break;
    case TerminalProfile::Command:
        m_command->setChecked(true);
        m_altShellSelection->setDisabled(true);
        m_commandText->setDisabled(false);
        m_runInShell->setDisabled(false);
        break;
    default:
        break;
    }

    for(int i = 0; i < m_altShellSelection->count(); ++i)
    {
        if(m_altShellSelection->itemText(i) ==
                p->alternativeShell())
            m_altShellSelection->setCurrentIndex(i);
    }
    m_exitAction->setCurrentIndex((int)p->tabCloseBehavior());

    switch(p->closeConsentBehavior())
    {
    case TerminalProfile::AskAlways:
        m_askAlways->setChecked(true);
        break;
    case TerminalProfile::AskNever:
        m_askNever->setChecked(true);
        break;
    case TerminalProfile::AskSelective:
        m_askSelective->setChecked(true);
        break;
    default:
        break;
    }

    m_selectiveCloseOptions->clear();
    m_selectiveCloseOptions->addItems(p->closeConsentExceptions());

    // Window Tab
    m_wndtitle->setText(p->windowTitle());
    auto flags = p->windowTitleFlags();

    if(flags & TerminalProfile::WorkingDirDocument)
    {
        m_wnddir->setChecked(true);
        m_wndpath->setEnabled(true);
    }
    if(flags & TerminalProfile::WorkingDirDocumentPath)
        m_wndpath->setChecked(true);
    if(flags & TerminalProfile::ActiveProcessName)
    {
        m_wndprocess->setChecked(true);
        m_wndargs->setEnabled(true);
    }
    if(flags & TerminalProfile::ActiveProcessArguments)
        m_wndargs->setChecked(true);
    if(flags & TerminalProfile::TTYName)
        m_wndtty->setChecked(true);
    if(flags & TerminalProfile::ShellCommandName)
        m_wndshellcmd->setChecked(true);
    if(flags & TerminalProfile::ProfileName)
        m_wndprofile->setChecked(true);
    if(flags & TerminalProfile::WindowDimensions)
        m_wnddimensions->setChecked(true);

    auto tflags = p->tabTitleFlags();
    if(tflags & TerminalProfile::WorkingDirDocumentTab)
    {
        m_tabdir->setChecked(true);
        m_tabpath->setEnabled(true);
    }
    if(tflags & TerminalProfile::WorkingDirDocumentPathTab)
        m_tabpath->setChecked(true);
    if(tflags & TerminalProfile::ActiveProcessNameTab)
    {
        m_tabproc->setChecked(true);
        m_tabargs->setEnabled(true);
    }
    if(tflags & TerminalProfile::ActiveProcessArgumentsTab)
        m_tabargs->setChecked(true);
    if(tflags & TerminalProfile::TTYNameTab)
        m_tabtty->setChecked(true);
    if(tflags & TerminalProfile::CustomTitleOverride)
        m_showothers->setChecked(true);
    if(tflags & TerminalProfile::ActivityIndicator)
        m_showactivity->setChecked(true);

    // Advanced Tab
    m_terminfo->setCurrentText(p->terminfo());
    for(int i = 0; i < m_terminfo->count(); ++i)
        if(m_terminfo->itemText(i) == p->terminfo())
            m_terminfo->setCurrentIndex(i);

    auto inputflags = p->inputFlags();
    if(inputflags & TerminalProfile::ScrollToBottom)
        m_scrollbottom->setChecked(true);

    switch(p->bellSound())
    {
    case TerminalProfile::CustomBellSound:
        m_custombell->setEnabled(true);
        m_customaudiobell->setChecked(true);
        break;
    case TerminalProfile::NoBellSound:
        m_noaudiobell->setChecked(true);
        m_custombell->setEnabled(false);
        break;
    case TerminalProfile::DefaultBellSound:
    default:
        m_custombell->setEnabled(false);
        m_defaultsoundbell->setChecked(true);
    }

    auto bellflags = p->bellFlags();
    if(bellflags & TerminalProfile::VisualBell)
    {
        m_visualbell->setChecked(true);
        m_vbellonlymute->setEnabled(true);
    }
    if(bellflags & TerminalProfile::VisualBellOnlyMuted)
        m_vbellonlymute->setChecked(true);
    if(bellflags & TerminalProfile::SystemNotification)
        m_notification->setChecked(true);
}

bool ProfileTab::renameProfile(const QString &oldProfile, const QString &newProfile)
{
    // TODO: move to application
    if(!checkProfileNameAvailable(newProfile))
        return false;

    QSettings settings("originull", "terminull");
    settings.beginGroup(QLatin1String("Profile-%1").arg(oldProfile));
    auto keys = settings.childKeys();
    settings.endGroup();
    foreach(auto key, keys)
    {
        settings.setValue(QLatin1String("Profile-%1/%2").arg(newProfile).arg(key),
          settings.value(QLatin1String("Profile-%1/%2").arg(oldProfile).arg(key)));
    }
    TerminalApplication::instance()->profileByName(oldProfile)->setProfileName(newProfile);
    settings.remove(QLatin1String("Profile-%1").arg(oldProfile));

    auto defProfile = TerminalApplication::instance()->defaultProfileName();
    if(defProfile == oldProfile)
        settings.setValue("DefaultProfile", newProfile);

    return true;
}

bool ProfileTab::checkProfileNameAvailable(const QString &name)
{
    QSettings settings("originull", "terminull");
    settings.beginGroup(QLatin1String("Profile-%1").arg(name));
    auto keys = settings.childKeys();
    settings.endGroup();
    if(keys.isEmpty())
        return true;

    return false;
}

QIcon ProfileTab::createColorIcon(const QColor &color)
{
    QPixmap pm(100,100);
    pm.fill(color);
    QIcon i(pm);
    return i;
}

void ProfileTab::listAltShells()
{
    QFile file("/etc/shells");
    if(file.open(QFile::ReadOnly))
    {
        auto shells = file.readAll().split('\n');
        for(auto shell : shells)
        {
            if(shell.startsWith('#'))
                continue;

            if(shell.startsWith('/'))
                m_altShellSelection->addItem(shell);
        }
    }
}

void ProfileTab::addProfile()
{
    bool valid = false;
    auto newName = tr("New Profile");
    if(!checkProfileNameAvailable(newName))
    {
        int i = 1;
        while(valid == false || i > 1000)
        {
            newName = tr("New Profile %1").arg(i);
            if(checkProfileNameAvailable(newName))
            {
                valid = true;
                break;
            }
        }
    }
    else
        valid = true;

    if(valid)
    {
        if(TerminalApplication::instance()->createProfile(newName))
        {
            auto profile = TerminalApplication::instance()->profileByName(newName);
            auto item = new QStandardItem(profile->previewIcon(), profile->profileName());
            item->setData(profile->profileName());
            m_remove->setDisabled(false);
            m_model->appendRow(item);
        }
    }
}

void ProfileTab::removeProfile()
{
    auto profileCount =
            TerminalApplication::instance()->profiles().count();

    if(profileCount < 2)
    {
        // we accidently wound up here - we can't delete
        // the last profile so bail out
        m_remove->setDisabled(true);
        qDebug() << "ProfileTab::removeProfile: bailing";
        return;
    }

    auto pname = m_selectedProfile->profileName();
    QMessageBox delq(this);
    delq.setWindowTitle(tr("Delete Profile?"));
    delq.setText(tr("Do you want to delete the profile %1?").arg(pname));
    delq.setIcon(QMessageBox::Question);
    delq.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    int result = delq.exec();
    if(result == QMessageBox::Yes)
    {
        //
        auto targetProfile = m_model->findItems(pname).first();
        if(targetProfile)
        {
            if(targetProfile->text() == m_selectedProfile->profileName())
            {
                if(TerminalApplication::instance()->removeProfile(pname))
                {
                    m_model->removeRow(m_model->indexFromItem(targetProfile).row());
                    m_selectedProfile = nullptr;
                    if(m_model->rowCount() < 2)
                        m_remove->setDisabled(true);
                }
            }
        }
    }
}

void ProfileTab::setDefaultProfile()
{
    QSettings settings("originull","terminull");
    settings.setValue("DefaultProfile", m_selectedProfile->profileName());
    m_setDefault->setDisabled(true);
}

void ProfileTab::selectColor()
{
    auto tbtn = qobject_cast<QToolButton*>(sender());
    Q_ASSERT(tbtn);
    if(tbtn == m_bgcolor)
    {
        m_currentEdit = Background;
        m_colordialog->setWindowTitle(tr("Select Background Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->backgroundColor());
    }
    if(tbtn == m_textcolor)
    {
        m_currentEdit = Text;
        m_colordialog->setWindowTitle(tr("Select Text Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->textColor());
    }
    if(tbtn == m_boldtextcolor)
    {
        m_currentEdit = BoldText;
        m_colordialog->setWindowTitle(tr("Select Bold Text Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->boldColor());
    }
    if(tbtn == m_selectioncolor)
    {
        m_currentEdit = Selection;
        m_colordialog->setWindowTitle(tr("Select Selection Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->selectionColor());
    }
    if(tbtn == m_curColor)
    {
        m_currentEdit = Cursor;
        m_colordialog->setWindowTitle(tr("Select Cursor Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->cursorColor());
    }

    if(tbtn == m_curColor)
    {
        m_currentEdit = Cursor;
        m_colordialog->setWindowTitle(tr("Select Cursor Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->cursorColor());
    }

    if(tbtn == m_ansiBlack)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Black Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Black));
        m_currentAnsiEdit = (int)TerminalProfile::Black;
    }

    if(tbtn == m_ansiRed)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Red Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Red));
        m_currentAnsiEdit = (int)TerminalProfile::Red;
    }

    if(tbtn == m_ansiGreen)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Green Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Green));
        m_currentAnsiEdit = (int)TerminalProfile::Green;
    }

    if(tbtn == m_ansiYellow)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Yellow Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Yellow));
        m_currentAnsiEdit = (int)TerminalProfile::Yellow;
    }

    if(tbtn == m_ansiBlue)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Blue Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Blue));
        m_currentAnsiEdit = (int)TerminalProfile::Blue;
    }

    if(tbtn == m_ansiMagenta)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Magenta Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Magenta));
        m_currentAnsiEdit = (int)TerminalProfile::Magenta;
    }

    if(tbtn == m_ansiCyan)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Cyan Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Cyan));
        m_currentAnsiEdit = (int)TerminalProfile::Cyan;
    }

    if(tbtn == m_ansiWhite)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI White Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::White));
        m_currentAnsiEdit = (int)TerminalProfile::White;
    }


    if(tbtn == m_ansiBrightBlack)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Black Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightBlack));
        m_currentAnsiEdit = (int)TerminalProfile::BrightBlack;
    }

    if(tbtn == m_ansiBrightRed)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Red Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightRed));
        m_currentAnsiEdit = (int)TerminalProfile::BrightRed;
    }

    if(tbtn == m_ansiBrightGreen)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Green Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::Green));
        m_currentAnsiEdit = (int)TerminalProfile::BrightGreen;
    }

    if(tbtn == m_ansiBrightYellow)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Yellow Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightYellow));
        m_currentAnsiEdit = (int)TerminalProfile::BrightYellow;
    }

    if(tbtn == m_ansiBrightBlue)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Blue Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightBlue));
        m_currentAnsiEdit = (int)TerminalProfile::BrightBlue;
    }

    if(tbtn == m_ansiBrightMagenta)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Magenta Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightBlack));
        m_currentAnsiEdit = (int)TerminalProfile::BrightMagenta;
    }

    if(tbtn == m_ansiBrightCyan)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright Cyan Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightCyan));
        m_currentAnsiEdit = (int)TerminalProfile::BrightCyan;
    }

    if(tbtn == m_ansiBrightWhite)
    {
        m_currentEdit = ANSI;
        m_colordialog->setWindowTitle(tr("Set ANSI Bright White Color"));
        m_colordialog->setCurrentColor(m_selectedProfile->ansiColor(TerminalProfile::BrightWhite));
        m_currentAnsiEdit = (int)TerminalProfile::BrightWhite;
    }

    m_colordialog->show();
}

void ProfileTab::showFontDialog()
{
    m_fontdialog->setCurrentFont(m_selectedProfile->font());
    m_fontdialog->show();
}

void ProfileTab::colorDialogChanged(const QColor &color)
{
    if(m_selectedProfile == nullptr)
        return;

    switch(m_currentEdit)
    {
    case Background:
        m_bgcolor->setIcon(createColorIcon(color));
        m_selectedProfile->setBackgroundColor(color);
        break;
    case Text:
        m_textcolor->setIcon(createColorIcon(color));
        m_selectedProfile->setTextColor(color);
        break;
    case BoldText:
        m_boldtextcolor->setIcon(createColorIcon(color));
        m_selectedProfile->setBoldColor(color);
        break;
    case Selection:
        m_selectioncolor->setIcon(createColorIcon(color));
        m_selectedProfile->setSelectionColor(color);
        break;
    case Cursor:
        m_curColor->setIcon(createColorIcon(color));
        m_selectedProfile->setCursorColor(color);
        break;
    case ANSI:
        auto val = (TerminalProfile::ANSIColors)m_currentAnsiEdit;
        switch(val)
        {
        case TerminalProfile::Black:
            m_ansiBlack->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::Red:
            m_ansiRed->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::Green:
            m_ansiGreen->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::Yellow:
            m_ansiYellow->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::Blue:
            m_ansiBlue->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::Magenta:
            m_ansiMagenta->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::Cyan:
            m_ansiCyan->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::White:
            m_ansiWhite->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightBlack:
            m_ansiBrightBlack->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightRed:
            m_ansiBrightRed->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightGreen:
            m_ansiBrightGreen->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightYellow:
            m_ansiBrightYellow->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightBlue:
            m_ansiBrightBlue->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightMagenta:
            m_ansiBrightMagenta->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightCyan:
            m_ansiBrightCyan->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        case TerminalProfile::BrightWhite:
            m_ansiBrightWhite->setIcon(createColorIcon(color));
            m_selectedProfile->setANSIColor(val,color);
            break;
        default:
            break;
        }
    }
}

void ProfileTab::colorDialogClosed(int result)
{
    Q_UNUSED(result);
    m_currentAnsiEdit = -1;
}

void ProfileTab::fontChanged(const QFont &font)
{
    if(m_selectedProfile == nullptr)
        return;

    m_selectedProfile->setFont(font);
    m_font->setFont(font);
    auto fontstr = QString("%1 %2pt").arg(font.family()).arg(font.pointSize());
    m_font->setText(fontstr);
}

void ProfileTab::selectBackgroundImage()
{
    QFileDialog od(this);
    od.setAcceptMode(QFileDialog::AcceptOpen);
    od.setFileMode(QFileDialog::ExistingFile);
    od.setWindowTitle(tr("Select Background Image"));
    od.setNameFilters({tr("Images (*.png, *.jpg)"), tr("All Files (*)")});
    auto result = od.exec();
    if(result == QDialog::Accepted)
    {
        auto fn = od.selectedFiles().first();
        QFile file(fn);
        qDebug() << "attempting to set bg" << fn;

        if(file.open(QFile::ReadOnly))
        {
            // good readable file, set it
            QSettings settings("originull", "terminull");
            m_selectedProfile->setBackgroundImage(fn);
            m_bgimage->setCurrentText(fn.split('/').last());
            qDebug() << fn;
            file.close();
            return;
        }
    }
    // restore our original selection (fallback)
     m_bgimage->setCurrentIndex(0);

}

void ProfileTab::profileModelRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    auto item = m_model->itemFromIndex(current);
    loadProfileData(item->text());
    if(item->text() == TerminalApplication::instance()->defaultProfileName())
        m_setDefault->setEnabled(false);
    else
        m_setDefault->setEnabled(true);
}

void ProfileTab::transparencySliderChanged(int value)
{
    if(m_selectedProfile == nullptr)
        return;

    qreal trans = value/100.0;
    qDebug() << trans;
    m_selectedProfile->setBackgroundTransparency(trans);
}

void ProfileTab::styleCheckboxChanged(bool checked)
{
    auto cb = qobject_cast<QCheckBox*>(sender());
    Q_ASSERT(cb);

    if(m_selectedProfile == nullptr)
        return;

    if(cb == m_bold)
        m_selectedProfile->setUseBold(checked);

    if(cb == m_blink)
        m_selectedProfile->setTextBlink(checked);

    if(cb == m_ansicolor)
        m_selectedProfile->setUseANSIColor(checked);

    if(cb == m_blinkCur)
        m_selectedProfile->setCursorBlink(checked);
}

void ProfileTab::cursorRadioChanged(bool checked)
{
    Q_UNUSED(checked);
    auto rb = qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(rb);

    if(m_selectedProfile == nullptr)
        return;

    if(rb == m_curblock)
        m_selectedProfile->setCursorType(TerminalProfile::Block);

    if(rb == m_curunderline)
        m_selectedProfile->setCursorType(TerminalProfile::Underline);

    if(rb == m_curibeam)
        m_selectedProfile->setCursorType(TerminalProfile::IBeam);
}

void ProfileTab::startupRadioChanged(bool checked)
{
    Q_UNUSED(checked);
    auto rb = qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(rb);

    if(m_selectedProfile == nullptr)
        return;

    if(rb == m_defaultshell)
    {
        m_selectedProfile->setStartupBehavior(TerminalProfile::LoginShell);
        m_altShellSelection->setDisabled(true);
        m_commandText->setDisabled(true);
        m_runInShell->setDisabled(true);
    }
    if(rb == m_altshell)
    {
        m_selectedProfile->setStartupBehavior(TerminalProfile::CustomShell);
        m_altShellSelection->setDisabled(false);
        m_commandText->setDisabled(true);
        m_runInShell->setDisabled(true);
    }
    if(rb == m_command)
    {
        m_selectedProfile->setStartupBehavior(TerminalProfile::Command);
        m_altShellSelection->setDisabled(true);
        m_commandText->setDisabled(false);
        m_runInShell->setDisabled(false);
    }
}

void ProfileTab::bellRadioChanged(bool checked)
{
    Q_UNUSED(checked);
    auto rb = qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(rb);

    if(m_selectedProfile == nullptr)
        return;

    if(rb == m_customaudiobell)
    {
        m_selectedProfile->setBellSound(TerminalProfile::CustomBellSound);
        m_custombell->setEnabled(true);
    }
    else
        m_custombell->setEnabled(false);

    if(rb == m_defaultsoundbell)
        m_selectedProfile->setBellSound(TerminalProfile::DefaultBellSound);

    if(rb == m_noaudiobell)
        m_selectedProfile->setBellSound(TerminalProfile::NoBellSound);
}

void ProfileTab::inputCheckChanged(bool checked)
{
    auto cb = qobject_cast<QCheckBox*>(sender());
    auto inputflags = m_selectedProfile->inputFlags();
    Q_ASSERT(cb);

    if(cb == m_scrollbottom)
    {
        qDebug() << m_scrollbottom;
        if(checked)
            inputflags & TerminalProfile::ScrollToBottom;
        else
            inputflags & ~TerminalProfile::ScrollToBottom;
    }
    m_selectedProfile->setInputFlags(inputflags);
}

void ProfileTab::windowTitleFlagCheckboxChanged(bool checked)
{
    Q_UNUSED(checked);
}

void ProfileTab::tabTitleFlagCheckboxChanged(bool checked)
{
    Q_UNUSED(checked);
}

void ProfileTab::quitConsentRadioChanged(bool checked)
{
    Q_UNUSED(checked);
    auto rb = qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(rb);

    if(m_selectedProfile == nullptr)
        return;

    if(rb == m_askAlways)
        m_selectedProfile->setCloseConsentBehavior(TerminalProfile::AskAlways);

    if(rb == m_askNever)
        m_selectedProfile->setCloseConsentBehavior(TerminalProfile::AskNever);

    if(rb == m_askSelective)
        m_selectedProfile->setCloseConsentBehavior(TerminalProfile::AskSelective);
}

void ProfileTab::onAltShellIndexChanged(int index)
{
    if(m_selectedProfile == nullptr)
        return;

    m_selectedProfile->setAlternativeShell(m_altShellSelection->itemText(index));
}

void ProfileTab::onOnExitIndexChanged(int index)
{
    if(m_selectedProfile == nullptr)
        return;

    m_selectedProfile->setTabCloseBehavior((TerminalProfile::TabCloseBehavior)index);
}

void ProfileTab::backgroundSelectorChanged(int index)
{
    QString data = m_bgimage->itemData(index).toString();
    if(data.isEmpty())
        m_bgimage->setCurrentIndex(0);

    if(data == "None")
    {
        m_selectedProfile->setBackgroundImage("");
        m_bgimage->setCurrentIndex(0);
    }

    if(data == "Select")
        return selectBackgroundImage();

    if(data == "Existing")
    {

    }
}

void ProfileTab::profileChanged(QStandardItem *item)
{
    auto oldTitle = item->data().toString();
    auto newTitle = item->text();
    if(oldTitle == newTitle)
        return;

    if(!renameProfile(oldTitle, newTitle))
    {
        //TODO: message box warning
        item->setText(oldTitle);
    }
    else
    {
        item->setData(newTitle);
    }
}
