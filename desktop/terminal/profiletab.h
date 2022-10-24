#ifndef PROFILETAB_H
#define PROFILETAB_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QToolButton>
#include <QStandardItemModel>
#include <QListView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QListWidget>
#include <QFormLayout>
#include <QLineEdit>

class QColorDialog;
class QFontDialog;
class TerminalProfile;
class ProfileTab : public QWidget
{
    Q_OBJECT
public:
    enum ColorSelections
    {
        Background,
        Text,
        BoldText,
        Selection,
        Cursor,
        ANSI
    };
    ProfileTab(QWidget *parent = nullptr);
    ~ProfileTab();
private:
    void setupStyleTab();
    void setupShellTab();
    void setupWindowTab();
    void setupAdvancedTab();
    void loadProfiles();
    void loadProfileData(const QString &profile);
    bool renameProfile(const QString &oldProfile, const QString &newProfile);
    bool checkProfileNameAvailable(const QString &name);
    QIcon createColorIcon(const QColor &color);
    void listAltShells();
private slots:
    void addProfile();
    void removeProfile();
    void setDefaultProfile();
    void selectColor();
    void showFontDialog();
    void colorDialogChanged(const QColor &color);
    void colorDialogClosed(int result);
    void fontChanged(const QFont &font);
    void selectBackgroundImage();
    void profileModelRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void transparencySliderChanged(int value);
    void styleCheckboxChanged(bool checked);
    void cursorRadioChanged(bool checked);
    void startupRadioChanged(bool checked);
    void bellRadioChanged(bool checked);
    void inputCheckChanged(bool checked);
    void windowTitleFlagCheckboxChanged(bool checked);
    void tabTitleFlagCheckboxChanged(bool checked);
    void quitConsentRadioChanged(bool checked);
    void onAltShellIndexChanged(int index);
    void onOnExitIndexChanged(int index);
    void backgroundSelectorChanged(int index);
    void profileChanged(QStandardItem *item);
private:
    TerminalProfile *m_selectedProfile = nullptr;
    ColorSelections m_currentEdit = Background;
    int m_currentAnsiEdit = -1;
    QColorDialog *m_colordialog = nullptr;
    QFontDialog *m_fontdialog = nullptr;

    QListView *m_profileList;
    QStandardItemModel *m_model;
    QToolButton *m_add;
    QToolButton *m_remove;
    QToolButton *m_options;
    QToolButton *m_setDefault;
    QTabWidget *m_tabs;

    // Style Tab
    QWidget *m_tabText;
    QVBoxLayout *vl_tabText;
    QGroupBox *gb_background;
    QToolButton *m_bgcolor;
    QComboBox *m_bgimage;
    QSlider *m_bgtrans;
    QGroupBox *gb_font;
    QLabel *m_font;
    QPushButton *m_chgfont;
    QGroupBox *gb_text;
    QToolButton *m_textcolor;
    QToolButton *m_boldtextcolor;
    QToolButton *m_selectioncolor;
    QCheckBox *m_bold;
    QCheckBox *m_blink;
    QCheckBox *m_ansicolor;
    QGroupBox *gb_colors;
    QToolButton *m_ansiBlack;
    QToolButton *m_ansiRed;
    QToolButton *m_ansiGreen;
    QToolButton *m_ansiYellow;
    QToolButton *m_ansiBlue;
    QToolButton *m_ansiMagenta;
    QToolButton *m_ansiCyan;
    QToolButton *m_ansiWhite;
    QToolButton *m_ansiBrightBlack;
    QToolButton *m_ansiBrightRed;
    QToolButton *m_ansiBrightGreen;
    QToolButton *m_ansiBrightYellow;
    QToolButton *m_ansiBrightBlue;
    QToolButton *m_ansiBrightMagenta;
    QToolButton *m_ansiBrightCyan;
    QToolButton *m_ansiBrightWhite;
    QGroupBox *gb_cursor;
    QRadioButton *m_curblock;
    QRadioButton *m_curunderline;
    QRadioButton *m_curibeam;
    QCheckBox *m_blinkCur;
    QToolButton *m_curColor;

    // Shell Tab
    QWidget *m_tabShell;
    QGroupBox *gb_startup;
    QRadioButton *m_defaultshell;
    QRadioButton *m_altshell;
    QRadioButton *m_command;
    QComboBox *m_altShellSelection;
    QLineEdit *m_commandText;
    QCheckBox *m_runInShell;
    QGroupBox *gb_exit;
    QComboBox *m_exitAction;
    QGroupBox *gb_askclose;
    QRadioButton *m_askAlways;
    QRadioButton *m_askNever;
    QRadioButton *m_askSelective;
    QListWidget *m_selectiveCloseOptions;

    // Window Tab
    QWidget *m_tabWindow;
    QGroupBox *gb_windowtitle;
    QLineEdit *m_wndtitle;
    QCheckBox *m_wnddir;
    QCheckBox *m_wndpath;
    QCheckBox *m_wndprocess;
    QCheckBox *m_wndargs;
    QCheckBox *m_wndshellcmd;
    QCheckBox *m_wndprofile;
    QCheckBox *m_wndtty;
    QCheckBox *m_wnddimensions;
    QGroupBox *gb_tabtitle;
    QCheckBox *m_tabdir;
    QCheckBox *m_tabpath;
    QCheckBox *m_tabproc;
    QCheckBox *m_tabargs;
    QCheckBox *m_tabtty;
    QCheckBox *m_showothers;
    QCheckBox *m_showactivity;

    // Advanced Tab
    QWidget *m_tabAdvanced;
    QGroupBox *gb_terminfo;
    QComboBox *m_terminfo;
    QGroupBox *gb_input;
    QCheckBox *m_pastecheck;
    QCheckBox *m_scrollbottom;
    QGroupBox *gb_bell;
    QRadioButton *m_noaudiobell;
    QRadioButton *m_defaultsoundbell;
    QRadioButton *m_customaudiobell;
    QComboBox *m_custombell;
    QCheckBox *m_visualbell;
    QCheckBox *m_vbellonlymute;
    QCheckBox *m_notification;
};

#endif // PROFILETAB_H
