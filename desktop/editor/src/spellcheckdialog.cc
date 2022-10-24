#include "spellcheckdialog.h"

SpellCheckDialog::SpellCheckDialog(QWidget *parent)
    :QDialog(parent),
     m_mainLayout(new QGridLayout(this)),
     m_suggestions(new QListWidget(this)),
     m_top_btn_layout(new QVBoxLayout),
     m_change(new QPushButton(this)),
     m_find_next(new QPushButton(this)),
     m_layout_error(new QVBoxLayout),
     m_word(new QLineEdit(this)),
     m_word_status(new QLabel(this)),
     m_layout_btns_2(new QVBoxLayout),
     m_ignore(new QPushButton(this)),
     m_learn(new QPushButton(this)),
     m_define(new QPushButton(this)),
     m_guess(new QPushButton(this)),
     m_spacer_side_btns(new QSpacerItem(2, 2, QSizePolicy::Minimum, QSizePolicy::Expanding)),
     m_middle_spacer(new QSpacerItem(5, 20, QSizePolicy::Minimum, QSizePolicy::Maximum))
{
    //resize(526, 326);
    setWindowIcon(QIcon());
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //setWindowFlag(Qt::Tool, true);
    setWindowTitle(tr("Check Spelling"));
    m_mainLayout->addWidget(m_suggestions, 2, 0, 1, 1);
    m_top_btn_layout->addWidget(m_change);
    m_top_btn_layout->addWidget(m_find_next);
    m_mainLayout->addLayout(m_top_btn_layout, 0, 1, 1, 1);
    m_layout_error->addWidget(m_word);
    m_layout_error->addWidget(m_word_status);
    m_mainLayout->addLayout(m_layout_error, 0, 0, 1, 1);
    m_layout_btns_2->addWidget(m_ignore);
    m_layout_btns_2->addWidget(m_learn);
    m_layout_btns_2->addWidget(m_define);
    m_layout_btns_2->addWidget(m_guess);
    m_layout_btns_2->addItem(m_spacer_side_btns);
    m_mainLayout->addLayout(m_layout_btns_2, 2, 1, 1, 1);
    m_mainLayout->addItem(m_middle_spacer, 1, 0, 1, 1);

    m_change->setText(tr("Change"));
    m_find_next->setText(tr("Find Next"));
    m_word_status->setText(tr("Word not found in dictionary."));
    m_ignore->setText(tr("Ignore"));
    m_learn->setText(tr("Learn"));
    m_define->setText(tr("Define"));
    m_guess->setText(tr("Guess"));

    connect(m_change, SIGNAL(pressed()), this, SLOT(change()));
    connect(m_learn, SIGNAL(pressed()), this, SLOT(learn()));
    connect(m_ignore, SIGNAL(pressed()), this, SLOT(ignore()));
    //connect(m_find_next, SIGNAL(pressed()), this, SLOT(findNext()));


#ifndef THINK_DIFFERENT
    m_define->setVisible(false);
#endif
    connect(m_suggestions, SIGNAL(currentTextChanged(QString)),
            this, SLOT(suggestionChanged(QString)));
}

SpellCheckAction SpellCheckDialog::checkWord(const QString &word)
{
    m_returncode = Abort;
    m_word->clear();
    m_suggestions->clear();
    m_replacement = QString();

    m_word->setText(word);
    m_startWord = word;
    QStringList sug = myApp->spellChecker()->suggestionsForWord(word);
    m_suggestions->addItems(sug);

    if(sug.count() > 0)
        m_suggestions->setCurrentRow(0, QItemSelectionModel::Select);

    exec();

    return m_returncode;
}

QString SpellCheckDialog::replacementWord()
{
    return m_replacement;
}

void SpellCheckDialog::suggestionChanged(QString &str)
{
    if(str.isEmpty())
        m_change->setEnabled(false);
    else
        m_change->setEnabled(true);
}

void SpellCheckDialog::change()
{
    m_replacement = m_suggestions->selectedItems().first()->text();
    m_returncode = ChangeWord;
    accept();
}

void SpellCheckDialog::ignore()
{
    myApp->spellChecker()->addWordToIgnore(m_startWord);
    m_returncode = IgnoreWord;
    accept();
}

void SpellCheckDialog::learn()
{
    myApp->spellChecker()->addWordToPersonalDictionary(m_startWord);
    m_returncode = LearnWord;
    accept();
}

void SpellCheckDialog::guess()
{

}
