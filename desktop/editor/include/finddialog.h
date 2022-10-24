#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include "editor.h"

#ifdef Q_WS_MAC
#include "mac/qbutton.h"
#endif

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindDialog(QWidget *parent = nullptr);
    QString findString() const;
    void updateFindSettings(FindSettings &settings);
signals:
    void findStringChanged(const QString &findString);
    void replaceStringChanged(const QString &replaceString);
    void findNext();
    void findPrevious();
    void findAll();
    void regexpChanged(bool checked);
    void wholeWordChanged(bool checked);
    void wrapAroundChanged(bool checked);
    void caseSensitiveChanged(bool checked);
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void show();
    void hide();
    void setFindString(const QString &findString);
private slots:
    void updateFindStringChanged();
    void launchFindReplaceHelp();
    void savePaletteState();
    void restorePaletteState();
private:
    QGridLayout *gridLayout;
    QLabel *lbl_find;
    QHBoxLayout *layout_find;
    QPlainTextEdit *m_find_txt;
    QVBoxLayout *layout_find_act;
    QToolButton *m_find_hist;
    QToolButton *m_template;
    QSpacerItem *verticalSpacer_3;
    QVBoxLayout *layout_find_btn;
    QPushButton *m_next;
    QPushButton *m_previous;
    QPushButton *m_findall;
    QSpacerItem *verticalSpacer;
    QLabel *lbl_replace;
    QHBoxLayout *layout_replace;
    QPlainTextEdit *m_replace_txt;
    QVBoxLayout *layout_replace_act;
    QToolButton *m_replace_hist;
    QSpacerItem *verticalSpacer_4;
    QVBoxLayout *layout_replace_btn;
    QPushButton *m_replace;
    QPushButton *m_replace_all;
    QPushButton *m_replace_find;
    QSpacerItem *verticalSpacer_2;
    QLabel *lbl_options;
    QHBoxLayout *layout_options;
    QVBoxLayout *verticalLayout;
    QCheckBox *m_case_sensitive;
    QCheckBox *m_wraparound;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *m_regular_expression;
    QCheckBox *m_whole_words;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *layout_status;
    QLabel *m_find_status;
    QSpacerItem *horizontalSpacer;
#ifdef Q_WS_MAC
    QButton *m_help;
#else
    QPushButton *m_help;
#endif
};

#endif // FINDDIALOG_H
