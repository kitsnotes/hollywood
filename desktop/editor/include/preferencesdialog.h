#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "editor.h"

#ifdef Q_WS_MAC
#include "mac/qbutton.h"
#endif

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
private slots:
    void loadSettings();
    void saveSettings();
private:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *lbl_font;
    QHBoxLayout *fontLayout;
    QLineEdit *m_fontPreview;
    QPushButton *m_changeFont;
    QCheckBox *m_showLineNumbers;
    QCheckBox *m_highlightLine;
    QCheckBox *m_visualize_white_space;
    QLabel *lbl_caretSize;
    QSlider *m_caretWidth;
    QFrame *line;
    QLabel *lbl_tab_size;
    QSpinBox *m_tabsize;
    QCheckBox *m_spacesTabs;
    QFrame *line1;
    QLabel *lbl_smart;
    QCheckBox *m_auto_indent;
    QCheckBox *m_wrap_selection;
    QFrame *line2;
    QLabel *lbl_spelling;
    QComboBox *cb_spelling;
    QFrame *line3;
    QLabel *lbl_encoding;
    QComboBox *cb_encoding;
    QLabel *lbl_line_endings;
    QVBoxLayout *vl_line_endings;
    QRadioButton *rb_unix;
    QRadioButton *rb_mac;
    QRadioButton *rb_windows;
    QSpacerItem *m_spacer;
    QDialogButtonBox *m_buttonbox;
    QButtonGroup *bg_line_endings;
};

#endif // PREFERENCESDIALOG_H
