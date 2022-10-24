
#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    :QDialog(parent),
     gridLayout(new QGridLayout(this)),
     lbl_find(new QLabel(this)),
     layout_find(new QHBoxLayout),
     m_find_txt(new QPlainTextEdit(this)),
     layout_find_act(new QVBoxLayout),
     m_find_hist(new QToolButton(this)),
     m_template(new QToolButton(this)),
     verticalSpacer_3(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding)),
     layout_find_btn(new QVBoxLayout),
     m_next(new QPushButton(this)),
     m_previous(new QPushButton(this)),
     m_findall(new QPushButton(this)),
     verticalSpacer(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                    QSizePolicy::Expanding)),
     lbl_replace(new QLabel(this)),
     layout_replace(new QHBoxLayout),
     m_replace_txt(new QPlainTextEdit(this)),
     layout_replace_act(new QVBoxLayout),
     m_replace_hist(new QToolButton(this)),
     verticalSpacer_4(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding)),
     layout_replace_btn(new QVBoxLayout),
     m_replace(new QPushButton(this)),
     m_replace_all(new QPushButton(this)),
     m_replace_find(new QPushButton(this)),
     verticalSpacer_2(new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding)),
     lbl_options(new QLabel(this)),
     layout_options(new QHBoxLayout),
     verticalLayout(new QVBoxLayout),
     m_case_sensitive(new QCheckBox(this)),
     m_wraparound(new QCheckBox(this)),
     verticalLayout_2(new QVBoxLayout),
     m_regular_expression(new QCheckBox(this)),
     m_whole_words(new QCheckBox(this)),
     horizontalSpacer_2(new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                        QSizePolicy::Minimum)),
     layout_status(new QHBoxLayout),
     m_find_status(new QLabel(this)),
     horizontalSpacer(new QSpacerItem(40, 20, QSizePolicy::Expanding,
                                      QSizePolicy::Minimum)),
#ifndef Q_WS_MAC
     m_help(new QPushButton(this))
#else
     m_help(new QButton(this, QButton::HelpButton))
#endif
{
    setWindowTitle(tr("Find & Replace"));
    setWindowIcon(QIcon());
#ifdef Q_WS_MAC
    setWindowFlag(Qt::Tool, true);
#else
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                   Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
#endif
    resize(688, 339);
    setSizeGripEnabled(true);
    lbl_find->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    gridLayout->addWidget(lbl_find, 0, 0, 1, 1);

    m_find_txt->setTabChangesFocus(true);
    m_find_txt->setUndoRedoEnabled(false);

    layout_find->addWidget(m_find_txt);
    layout_find_act->addWidget(m_find_hist);
    layout_find_act->addWidget(m_template);
    layout_find_act->addItem(verticalSpacer_3);
    layout_find->addLayout(layout_find_act);

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_next->sizePolicy().hasHeightForWidth());
    m_next->setSizePolicy(sizePolicy);
    m_next->setMinimumSize(QSize(150, 0));

    layout_find_btn->addWidget(m_next);
    layout_find_btn->addWidget(m_previous);
    layout_find_btn->addWidget(m_findall);
    layout_find_btn->addItem(verticalSpacer);
    layout_find->addLayout(layout_find_btn);

    gridLayout->addLayout(layout_find, 0, 1, 1, 1);

    lbl_replace->setMinimumSize(QSize(80, 0));
    lbl_replace->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    gridLayout->addWidget(lbl_replace, 1, 0, 1, 1);

    m_replace_txt->setTabChangesFocus(true);
    m_replace_txt->setUndoRedoEnabled(false);

    layout_replace->addWidget(m_replace_txt);
    layout_replace_act->addWidget(m_replace_hist);
    layout_replace_act->addItem(verticalSpacer_4);
    layout_replace->addLayout(layout_replace_act);

    layout_replace_btn->setSizeConstraint(QLayout::SetDefaultConstraint);
    m_replace->setMinimumSize(QSize(70, 0));

    layout_replace_btn->addWidget(m_replace);

    layout_replace_btn->addWidget(m_replace_all);
    m_replace_find->setMinimumSize(QSize(150, 0));

    layout_replace_btn->addWidget(m_replace_find);

    layout_replace_btn->addItem(verticalSpacer_2);
    layout_replace->addLayout(layout_replace_btn);

    gridLayout->addLayout(layout_replace, 1, 1, 1, 1);

    lbl_options->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    gridLayout->addWidget(lbl_options, 2, 0, 1, 1);
    verticalLayout->addWidget(m_case_sensitive);
    verticalLayout->addWidget(m_wraparound);
    layout_options->addLayout(verticalLayout);
    verticalLayout_2->addWidget(m_regular_expression);
    verticalLayout_2->addWidget(m_whole_words);
    layout_options->addLayout(verticalLayout_2);
    layout_options->addItem(horizontalSpacer_2);
    gridLayout->addLayout(layout_options, 2, 1, 1, 1);
    layout_status->addWidget(m_find_status);
    layout_status->addItem(horizontalSpacer);
    layout_status->addWidget(m_help);
    gridLayout->addLayout(layout_status, 3, 1, 1, 1);

    lbl_find->setText(tr("Find:"));
    lbl_replace->setText(tr("Replace:"));

    m_find_hist->setToolTip(tr("Find History"));
    m_find_hist->setIcon(QIcon(":/IconActions/SearchHistory"));

    m_template->setText(tr("..."));
    m_next->setText(tr("Find Next"));
    m_previous->setText(tr("Find Previous"));
    m_findall->setText(tr("Find All"));
    m_replace_hist->setToolTip(tr("Replace History"));
    m_replace_hist->setIcon(QIcon(":/IconActions/SearchHistory"));

    m_replace->setText(tr("Replace"));
    m_replace_all->setText(tr("Replace All"));
    m_replace_find->setText(tr("Replace && Find"));
    lbl_options->setText(tr("Options:"));
    m_case_sensitive->setText(tr("Case sensitive"));
    m_wraparound->setText(tr("Wrap around"));
    m_regular_expression->setText(tr("Use regular expressions"));
    m_whole_words->setText(tr("Whole words only"));

#ifdef Q_WS_MAC
    lbl_find->setAlignment(Qt::AlignTop|Qt::AlignRight);
    lbl_replace->setAlignment(Qt::AlignTop|Qt::AlignRight);
    lbl_options->setAlignment(Qt::AlignTop|Qt::AlignRight);
#else
    m_help->setText(tr("?"));
#endif

    connect(m_next, SIGNAL(pressed()), this, SIGNAL(findNext()));
    connect(m_previous, SIGNAL(pressed()), this, SIGNAL(findPrevious()));
    connect(m_findall, SIGNAL(pressed()), this, SIGNAL(findAll()));

    connect(m_help, SIGNAL(pressed()), this, SLOT(launchFindReplaceHelp()));
    connect(m_find_txt, SIGNAL(textChanged()),
            this, SLOT(updateFindStringChanged()));

    connect(m_case_sensitive, SIGNAL(toggled(bool)), this,
            SIGNAL(caseSensitiveChanged(bool)));
    connect(m_whole_words, SIGNAL(toggled(bool)), this,
            SIGNAL(wholeWordChanged(bool)));
    connect(m_wraparound, SIGNAL(toggled(bool)), this,
            SIGNAL(wrapAroundChanged(bool)));
    connect(m_regular_expression, SIGNAL(toggled(bool)), this,
            SIGNAL(regexpChanged(bool)));

#ifdef Q_WS_WIN
    BOOL dwmDisableEffects = TRUE;
    DwmSetWindowAttribute((HWND)winId(),
                    DWMWA_TRANSITIONS_FORCEDISABLED,
                    &dwmDisableEffects,
                    sizeof(dwmDisableEffects));
#endif
    restorePaletteState();
}

void FindDialog::updateFindStringChanged()
{
    const QString str = m_find_txt->toPlainText();
    emit findStringChanged(str);
}

void FindDialog::launchFindReplaceHelp()
{
    // TODO: Launch .chm or applehelp for the topic
}

void FindDialog::setFindString(const QString &findString)
{
    m_find_txt->setPlainText(findString);
}

QString FindDialog::findString() const
{
    return m_find_txt->toPlainText();
}

void FindDialog::updateFindSettings(FindSettings &settings)
{
    m_find_txt->setPlainText(settings.find_str);
    m_replace_txt->setPlainText(settings.replace_str);
    m_case_sensitive->setChecked(settings.case_sensitive);
    m_wraparound->setChecked(settings.wrap_around);
    m_whole_words->setChecked(settings.whole_words);
    m_regular_expression->setChecked(settings.regexp);
}

void FindDialog::show()
{
    restorePaletteState();
    QDialog::show();
}

void FindDialog::hide()
{
    savePaletteState();
    QDialog::hide();
}

void FindDialog::closeEvent(QCloseEvent *event)
{
    savePaletteState();
    accept();
    event->accept();
}

void FindDialog::savePaletteState()
{
    QSettings settings("originull", "persistence");
    settings.beginGroup("StarText");
    settings.setValue("FindReplacePos", saveGeometry());
    settings.endGroup();
}

void FindDialog::restorePaletteState()
{
    QSettings settings("originull", "persistence");

    settings.beginGroup("StarText");

    if(settings.contains("FindReplacePos"))
        restoreGeometry(settings.value("FindReplacePos").toByteArray());

    settings.endGroup();
}
