#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include "editor.h"

class ColorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ColorDialog(QWidget *parent = Q_NULLPTR);
    ~ColorDialog();
protected slots:
    void accept();
private slots:
    void configureFontVariable();
    void configureColor();
    void readSettings();
private:
    QColor getCurrentColorForButton(QToolButton* btn);
    QIcon getIconForColor(QColor c);
private:
    QVBoxLayout *mainLayout;
    QFormLayout *layout_fg;
    QHBoxLayout *def_layout;
    QLabel *lbl_default;
    QToolButton *m_defaultColor;
    QFormLayout *layout_bg;
    QLabel *lbl_default_bg;
    QToolButton *m_defaultBackground;
    QFrame *line;
    QGroupBox *m_groupBox;
    QFormLayout *formLayout_2;
    QLabel *labelStrings;
    QHBoxLayout *layoutString;
    QToolButton *m_colorStrings;
    QToolButton *m_boldStrings;
    QToolButton *m_italicString;
    QToolButton *m_underlineString;
    QHBoxLayout *layoutComments;
    QLabel *labelComments;
    QToolButton *m_colorComments;
    QToolButton *m_boldComments;
    QToolButton *m_italicComments;
    QToolButton *m_underlineComments;
    QLabel *lbl_keywords;
    QHBoxLayout *layoutKeywords;
    QToolButton *m_colorKeywords;
    QToolButton *m_boldKeywords;
    QToolButton *m_italicKeywords;
    QToolButton *m_underlineKeywords;
    QLabel *lbl_classes_tags;
    QHBoxLayout *layoutClasses;
    QToolButton *m_colorClasses;
    QToolButton *m_boldClasses;
    QToolButton *m_italicClasses;
    QToolButton *m_underlineClasses;
    QLabel *lbl_functions_parameters;
    QHBoxLayout *layoutFunctions;
    QToolButton *m_colorFunctions;
    QToolButton *m_boldFunctions;
    QToolButton *m_italicFunctions;
    QToolButton *m_underlineFunctions;
    QDialogButtonBox *m_buttons;
    QSpacerItem *spacer;

};

#endif // COLORDIALOG_H
