#ifndef SPELLCHECKDIALOG_H
#define SPELLCHECKDIALOG_H

#include "editor.h"

#ifdef Q_WS_MAC
#include "mac/qbutton.h"
#endif

class SpellCheck;
class SpellCheckDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SpellCheckDialog(QWidget *parent = Q_NULLPTR);
    SpellCheckAction checkWord(const QString &word);
    QString replacementWord();
private slots:
    void change();
    void ignore();
    void learn();
    void guess();
    void suggestionChanged(QString &str);
private:
    QGridLayout *m_mainLayout;
    QListWidget *m_suggestions;
    QVBoxLayout *m_top_btn_layout;
    QPushButton *m_change;
    QPushButton *m_find_next;
    QVBoxLayout *m_layout_error;
    QLineEdit *m_word;
    QLabel *m_word_status;
    QVBoxLayout *m_layout_btns_2;
    QPushButton *m_ignore;
    QPushButton *m_learn;
    QPushButton *m_define;
    QPushButton *m_guess;
    QSpacerItem *m_spacer_side_btns;
    QSpacerItem *m_middle_spacer;

private:
    SpellCheckAction m_returncode;
    QString m_replacement;
    QString m_startWord;
};

#endif // SPELLCHECKDIALOG_H
