#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>
#include <QFont>
#include <QColor>
#include <QMap>

class TerminalProfile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString profileName READ profileName WRITE setProfileName NOTIFY profileNameChanged);
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle NOTIFY windowTitleChanged);
    Q_PROPERTY(WindowTitleFlags windowTitleFlags READ windowTitleFlags WRITE setWindowTitleFlags NOTIFY windowTitleFlagsChanged);
    Q_PROPERTY(TabTitleFlags tabTitleFlags READ tabTitleFlags WRITE setTabTitleFlags NOTIFY tabTitleFlagsChanged);
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged);
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged);
    Q_PROPERTY(QColor boldColor READ boldColor WRITE setBoldColor NOTIFY boldColorChanged);
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY selectionColorChanged);
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged);
    Q_PROPERTY(QString backgroundImage READ backgroundImage WRITE setBackgroundImage NOTIFY backgroundImageChanged);
    Q_PROPERTY(qreal backgroundTransparency READ backgroundTransparency WRITE setBackgroundTransparency NOTIFY backgroundTransparencyChanged);
    Q_PROPERTY(bool useBold READ useBold WRITE setUseBold NOTIFY useBoldChanged);
    Q_PROPERTY(bool textBlink READ textBlink WRITE setTextBlink NOTIFY textBlinkChanged);
    Q_PROPERTY(bool useANSIColor READ useANSIColor WRITE setUseANSIColor NOTIFY useANSIColorChanged);
    Q_PROPERTY(CursorType cursorType READ cursorType WRITE setCursorType NOTIFY cursorTypeChanged);
    Q_PROPERTY(bool cursorBlink READ cursorBlink WRITE setCursorBlink NOTIFY cursorBlinkChanged);
    Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor NOTIFY cursorColorChanged);
    Q_PROPERTY(StartupBehavior startupBehavior READ startupBehavior WRITE setStartupBehavior NOTIFY startupBehaviorChanged);
    Q_PROPERTY(TabCloseBehavior tabCloseBehavior READ tabCloseBehavior WRITE setTabCloseBehavior NOTIFY tabCloseBehaviorChanged);
    Q_PROPERTY(CloseConsentBehavior closeConsentBehavior READ closeConsentBehavior WRITE setCloseConsentBehavior NOTIFY closeConsentBehaviorChanged);
    Q_PROPERTY(QStringList closeConsentExceptions READ closeConsentExceptions WRITE setCloseConsentExceptions NOTIFY closeConsentExceptionsChanged);
    Q_PROPERTY(AudibleBellSettings bellSound READ bellSound WRITE setBellSound NOTIFY bellSoundChanged);
    Q_PROPERTY(QString customBellSound READ customBellSound WRITE setCustomBellSound NOTIFY customBellSoundChanged);
    Q_PROPERTY(BellFlags bellFlags READ bellFlags WRITE setBellFlags NOTIFY bellFlagsChanged);
    Q_PROPERTY(QString terminfo READ terminfo WRITE setTerminfo NOTIFY terminfoChanged);

public:
    enum CursorType {
        Block = 0,
        Underline = 1,
        IBeam = 2
    };
    enum ANSIColors {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        BrightBlack,
        BrightRed,
        BrightGreen,
        BrightYellow,
        BrightBlue,
        BrightMagenta,
        BrightCyan,
        BrightWhite
    };
    enum StartupBehavior {
        LoginShell,
        CustomShell,
        Command
    };
    enum TabCloseBehavior {
        CloseAlways,
        CloseIfClean,
        DontClose
    };
    enum CloseConsentBehavior {
        AskAlways,
        AskNever,
        AskSelective
    };
    enum WindowTitleFlag {
        NoTitleFlags = 0,
        WorkingDirDocument = 1,
        WorkingDirDocumentPath = 2,
        ActiveProcessName = 4,
        ActiveProcessArguments = 8,
        TTYName = 16,
        ShellCommandName = 32,
        ProfileName = 64,
        WindowDimensions = 128
    };
    Q_DECLARE_FLAGS(WindowTitleFlags, WindowTitleFlag);
    enum TabTitleFlag {
        NoTtileFlag = 0,
        WorkingDirDocumentTab = 1,
        WorkingDirDocumentPathTab = 2,
        ActiveProcessNameTab = 4,
        ActiveProcessArgumentsTab = 8,
        TTYNameTab = 16,
        CustomTitleOverride = 32,
        ActivityIndicator = 64
    };
    Q_DECLARE_FLAGS(TabTitleFlags, TabTitleFlag);
    enum AudibleBellSettings {
        NoBellSound,
        DefaultBellSound,
        CustomBellSound
    };
    enum BellFlag {
        NoBellFlag = 0,
        VisualBell = 1,
        VisualBellOnlyMuted = 2,
        SystemNotification = 4
    };
    Q_DECLARE_FLAGS(BellFlags, BellFlag);
    enum InputFlag {
        NoInputFlag = 0,
        ScrollToBottom = 1
    };
    Q_DECLARE_FLAGS(InputFlags, InputFlag);

    explicit TerminalProfile();
    bool loadProfileFromSettings(const QString &profileName);
    void saveProfileToSettings();
    QIcon previewIcon() const;
public slots:
    void setProfileName(const QString &name);
    void setWindowTitle(const QString &title);
    void setWindowTitleFlags(const WindowTitleFlags &flags);
    void setTabTitleFlags(const TabTitleFlags &flags);
    void setFont(const QFont &font);
    void setUseBold(const bool &bold);
    void setTextBlink(const bool &blink);
    void setUseANSIColor(const bool &color);
    void setTextColor(const QColor &color);
    void setBoldColor(const QColor &color);
    void setSelectionColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setBackgroundImage(const QString &file);
    void setBackgroundTransparency(const qreal &trans);
    void setCursorType(const CursorType &type);
    void setCursorColor(const QColor &color);
    void setCursorBlink(const bool &blink);
    void setStartupBehavior(const StartupBehavior &behavior);
    void setAlternativeShell(const QString &shell);
    void setCustomCommand(const QString &cmd);
    void setANSIColor(const ANSIColors &ansi, const QColor &color);
    void setTabCloseBehavior(const TabCloseBehavior &behavior);
    void setCloseConsentBehavior(const CloseConsentBehavior &behavior);
    void setCloseConsentExceptions(const QStringList &exceptions);
    void setBellSound(const AudibleBellSettings &bell);
    void setBellFlags(const BellFlags &bell);
    void setCustomBellSound(const QString &file);
    void setTerminfo(const QString &terminfo);
    void setInputFlags(const InputFlags &flags);
public:
    QString profileName() const;
    QString windowTitle() const;

    WindowTitleFlags windowTitleFlags() const;
    TabTitleFlags tabTitleFlags() const;
    QFont font() const;

    bool useBold() const;
    bool textBlink() const;
    bool useANSIColor() const;
    QColor textColor() const;
    QColor boldColor() const;
    QColor selectionColor() const;
    QColor backgroundColor() const;
    QString backgroundImage() const;
    qreal backgroundTransparency() const;
    CursorType cursorType() const;
    QColor cursorColor() const;
    QColor ansiColor(ANSIColors color) const;
    bool cursorBlink() const;
    StartupBehavior startupBehavior() const;
    QString alternativeShell() const;
    QString customCommand() const;
    TabCloseBehavior tabCloseBehavior() const;
    CloseConsentBehavior closeConsentBehavior() const;
    QStringList closeConsentExceptions() const;
    AudibleBellSettings bellSound() const;
    BellFlags bellFlags() const;
    QString customBellSound() const;
    QString terminfo() const;
    InputFlags inputFlags() const;
Q_SIGNALS:
    void profileChanged();
    void profileNameChanged(const QString &name);
    void windowTitleChanged(const QString &title);
    void windowTitleFlagsChanged(const WindowTitleFlags flags);
    void tabTitleFlagsChanged(const TabTitleFlags flags);
    void fontChanged(const QFont &font);

    void useBoldChanged(const bool &bold);
    void textBlinkChanged(const bool &blink);
    void useANSIColorChanged(const bool &color);
    void textColorChanged(const QColor &color);
    void boldColorChanged(const QColor &color);
    void selectionColorChanged(const QColor &color);
    void backgroundColorChanged(const QColor &color);
    void backgroundTransparencyChanged(const qreal &trans);
    void backgroundImageChanged(const QString &file);
    void cursorTypeChanged(const CursorType &type);
    void cursorColorChanged(const QColor &color);
    void cursorBlinkChanged(const bool &blink);
    void startupBehaviorChanged(const StartupBehavior &behavior);
    void tabCloseBehaviorChanged(const TabCloseBehavior &behavior);
    void closeConsentBehaviorChanged(const CloseConsentBehavior &behavior);
    void closeConsentExceptionsChanged(const QStringList &exceptions);
    void bellSoundChanged(const AudibleBellSettings &bell);
    void bellFlagsChanged(const BellFlags &bell);
    void customBellSoundChanged(const QString &file);
    void terminfoChanged(const QString &terminfo);
    void inputFlagsChanged(const InputFlags &flags);
    void ANSIColorChanged(const ANSIColors &ansi, const QColor &color);
private:
    QString m_profilename;
    WindowTitleFlags m_wndflags;
    TabTitleFlags m_tabflags;
    AudibleBellSettings m_bellsound;
    QString m_bellsoundfile;
    BellFlags m_bellflags;

    QString m_terminfo;
    InputFlags m_inputflags;

    QFont m_font;
    QColor m_textcolor;
    QColor m_boldcolor;
    QColor m_selcolor;
    QColor m_bgcolor;
    qreal m_bgtrans;
    QString m_bgimg;

    bool m_bold;
    bool m_blinkText;
    bool m_ansicolor;

    QString m_windowtitle;
    QMap<ANSIColors,QColor> m_ansi;

    CursorType m_cursor;
    QColor m_cursorcolor;
    bool m_blinkCursor;

    StartupBehavior m_startup;
    QString m_altshell;
    QString m_customcommand;
    TabCloseBehavior m_close;
    CloseConsentBehavior m_ask;
    QStringList m_askexceptions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TerminalProfile::WindowTitleFlags);
Q_DECLARE_OPERATORS_FOR_FLAGS(TerminalProfile::TabTitleFlags);
Q_DECLARE_OPERATORS_FOR_FLAGS(TerminalProfile::BellFlags);

#endif // PROFILE_H
