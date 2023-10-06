#include "profile.h"

#include <QDebug>
#include <QApplication>
#include <QPalette>
#include <QSettings>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

TerminalProfile::TerminalProfile()
    : QObject(nullptr)
{
    m_profilename = tr("Untitled");
    m_font = QFont(QLatin1String("monospace"), 12);
    m_textcolor = QColor(255,255,255);
    m_boldcolor = QColor(255,255,255);
    m_bgcolor = QColor(0,0,0);
    QPalette p = qApp->palette();
    m_selcolor = qApp->palette().color(QPalette::Active, QPalette::Highlight);

    m_inputflags = ScrollToBottom;
    m_bold = true;
    m_blinkText = true;
    m_ansicolor = true;

    m_bgimg = QString();
    m_bgtrans = 1.0;

    m_windowtitle = QString("Terminull");
    m_wndflags = WorkingDirDocument|
                 ActiveProcessName|
                 ShellCommandName;

    m_tabflags = WorkingDirDocumentTab|
                 WorkingDirDocumentPathTab|
                 ActiveProcessNameTab|ActiveProcessArgumentsTab|
                 ActivityIndicator;

    m_ansi[Black] = QColor(Qt::darkGray);
    m_ansi[Red] = QColor(Qt::darkRed);
    m_ansi[Green] = QColor(Qt::darkGreen);
    m_ansi[Yellow] = QColor(Qt::darkYellow);
    m_ansi[Blue] = QColor(Qt::darkBlue);
    m_ansi[Magenta] = QColor(Qt::darkMagenta);
    m_ansi[Cyan] = QColor(Qt::darkCyan);
    m_ansi[White] = QColor(Qt::gray);

    m_ansi[BrightBlack] = QColor(Qt::black);
    m_ansi[BrightRed] = QColor(Qt::red);
    m_ansi[BrightGreen] = QColor(Qt::green);
    m_ansi[BrightYellow] = QColor(Qt::yellow);
    m_ansi[BrightBlue] = QColor(Qt::blue);
    m_ansi[BrightMagenta] = QColor(Qt::magenta);
    m_ansi[BrightCyan] = QColor(Qt::cyan);
    m_ansi[BrightWhite] = QColor(Qt::white);

    m_cursor = Block;
    m_cursorcolor = QColor(255,0,0);

    m_startup = LoginShell;
    m_close = DontClose;
    m_ask = AskSelective;

    m_askexceptions.append("screen");
    m_askexceptions.append("tmux");

    m_terminfo = QLatin1String("xterm-256color");
    m_bellsound = DefaultBellSound;
    m_bellflags = VisualBell|VisualBellOnlyMuted|SystemNotification;
}

bool TerminalProfile::loadProfileFromSettings(const QString &profileName)
{
    QSettings settings("originull", "terminull");
    if(!settings.childGroups().contains(QLatin1String("Profile-%1").arg(profileName)))
        return false;

    settings.beginGroup(QLatin1String("Profile-%1").arg(profileName));
    auto list = settings.group().split("-");
    list.removeFirst();
    m_profilename = list.join("-");
    m_font = QFont(QLatin1String("monospace"), 12);
    auto fontstr = m_font.toString();
    m_font.fromString(settings.value("Font", fontstr).toString());

    m_textcolor = settings.value("TextColor").toString();
    m_boldcolor = settings.value("BoldColor").toString();
    m_selcolor = settings.value("SelectionColor").toString();
    m_bgcolor = settings.value("BackgroundColor").toString();
    m_bgtrans = settings.value("BackgroundTransparency").toReal();
    m_bgimg = settings.value("BackgroundImage").toString();

    m_bold = settings.value("UseBoldText", true).toBool();
    m_blinkText = settings.value("BlinkText", true).toBool();
    m_ansicolor = settings.value("UseANSIColors", true).toBool();

    m_ansi[Black] = settings.value("ANSIColorBlack", QColor(Qt::darkGray)).toString();
    m_ansi[Red] = settings.value("ANSIColorRed", QColor(Qt::darkRed)).toString();
    m_ansi[Green] = settings.value("ANSIColorGreen", QColor(Qt::darkGreen)).toString();
    m_ansi[Yellow] = settings.value("ANSIColorYellow", QColor(Qt::darkYellow)).toString();
    m_ansi[Blue] = settings.value("ANSIColorBlue", QColor(Qt::darkBlue)).toString();
    m_ansi[Magenta] = settings.value("ANSIColorMagenta", QColor(Qt::darkMagenta)).toString();
    m_ansi[Cyan] = settings.value("ANSIColorCyan", QColor(Qt::darkCyan)).toString();
    m_ansi[White] = settings.value("ANSIColorWhite", QColor(Qt::gray)).toString();

    m_ansi[BrightBlack] = settings.value("ANSIColorBrightBlack", QColor(Qt::black)).toString();
    m_ansi[BrightRed] = settings.value("ANSIColorBrightRed", QColor(Qt::red)).toString();
    m_ansi[BrightGreen] = settings.value("ANSIColorBrightGreen", QColor(Qt::green)).toString();
    m_ansi[BrightYellow] = settings.value("ANSIColorBrightYellow", QColor(Qt::yellow)).toString();
    m_ansi[BrightBlue] = settings.value("ANSIColorBrightBlue", QColor(Qt::blue)).toString();
    m_ansi[BrightMagenta] = settings.value("ANSIColorBrightMagenta", QColor(Qt::magenta)).toString();
    m_ansi[BrightCyan] = settings.value("ANSIColorBrightCyan", QColor(Qt::cyan)).toString();
    m_ansi[BrightWhite] = settings.value("ANSIColorBrightWhite", QColor(Qt::white)).toString();

    int cursortype = settings.value("CursorType", 0).toInt();
    if(cursortype < 0)
        cursortype = 0;
    if(cursortype > 2)
        cursortype = 0;
    m_cursor = (CursorType)cursortype;
    m_cursorcolor = settings.value("CursorColor").toString();
    m_blinkCursor = settings.value("BlinkCursor", true).toBool();

    // Shell Settings
    m_startup = (StartupBehavior)settings.value("StartupBehavior").toInt();
    m_close = (TabCloseBehavior)settings.value("TabCloseBehavior").toInt();
    m_ask = (CloseConsentBehavior)settings.value("CloseConsent").toInt();
    m_askexceptions = settings.value("ConsentExcept").toStringList();
    m_altshell = settings.value("AltShell").toString();
    m_customcommand = settings.value("StartupCommand").toString();

    // Window settings
    m_windowtitle = settings.value("WindowTitle").toString();
    m_wndflags = (WindowTitleFlags)settings.value("WindowTitleFlags").toInt();
    m_tabflags = (TabTitleFlags)settings.value("TabTitleFlags").toInt();

    // Advanced settings
    m_terminfo = settings.value("TERM").toString();
    m_inputflags = (InputFlags)settings.value("InputFlags").toInt();
    m_bellflags = (BellFlags)settings.value("BellFlags").toInt();
    m_bellsoundfile = settings.value("BellSoundFile").toString();
    m_bellsound = (AudibleBellSettings)settings.value("AudibleBell").toInt();

    settings.endGroup();
    return true;
}

void TerminalProfile::saveProfileToSettings()
{
    // TODO: remove profile if renamed
    qDebug() << "TerminalProfile::saveProfileToSettings saving profile" << m_profilename;

    QSettings settings("originull", "terminull");
    settings.beginGroup(QLatin1String("Profile-%1").arg(m_profilename));

    // Style Settings
    settings.setValue("Font", m_font.toString());
    settings.setValue("TextColor", m_textcolor);
    settings.setValue("BoldColor", m_boldcolor);
    settings.setValue("SelectionColor", m_selcolor);
    settings.setValue("BackgroundColor", m_bgcolor);
    settings.setValue("BackgroundTransparency", m_bgtrans);
    settings.setValue("BackgroundImage", m_bgimg);

    settings.setValue("UseBoldText", m_bold);
    settings.setValue("BlinkText", m_blinkText);
    settings.setValue("UseANSIColors", m_ansicolor);

    settings.setValue("CursorType", m_cursor);
    settings.setValue("BlinkCursor", m_blinkCursor);
    settings.setValue("CursorColor", m_cursorcolor);

    settings.setValue("ANSIColorBlack", m_ansi[Black]);
    settings.setValue("ANSIColorRed", m_ansi[Red]);
    settings.setValue("ANSIColorGreen", m_ansi[Green]);
    settings.setValue("ANSIColorYellow", m_ansi[Yellow]);
    settings.setValue("ANSIColorBlue", m_ansi[Blue]);
    settings.setValue("ANSIColorMagentaa", m_ansi[Magenta]);
    settings.setValue("ANSIColorCyan", m_ansi[Cyan]);
    settings.setValue("ANSIColorWhite", m_ansi[White]);

    settings.setValue("ANSIColorBrightBlack", m_ansi[BrightBlack]);
    settings.setValue("ANSIColorBrightRed", m_ansi[BrightRed]);
    settings.setValue("ANSIColorBrightGreen", m_ansi[BrightGreen]);
    settings.setValue("ANSIColorBrightYellow", m_ansi[BrightYellow]);
    settings.setValue("ANSIColorBrightBlue", m_ansi[BrightBlue]);
    settings.setValue("ANSIColorBrightMagentaa", m_ansi[BrightMagenta]);
    settings.setValue("ANSIColorBrightCyan", m_ansi[BrightCyan]);
    settings.setValue("ANSIColorBrightWhite", m_ansi[BrightWhite]);

    // Shell Settings
    settings.setValue("StartupBehavior", m_startup);
    settings.setValue("AltShell", m_altshell);
    settings.setValue("StartupCommand", m_customcommand);
    settings.setValue("TabCloseBehavior", m_close);
    settings.setValue("CloseConsent", m_ask);
    settings.setValue("ConsentExcept", m_askexceptions);

    // Window Settings
    settings.setValue("WindowTitle", m_windowtitle);
    settings.setValue("WindowTitleFlags", (int)m_wndflags);
    settings.setValue("TabTitleFlags", (int)m_tabflags);

    // Advanced Settings
    settings.setValue("TERM", m_terminfo);
    settings.setValue("InputFlags", (int)m_inputflags);
    settings.setValue("AudibleBell", (int)m_bellsound);
    settings.setValue("BellFlags", (int)m_bellflags);
    settings.setValue("BellSoundFile", m_bellsoundfile);
    settings.endGroup();
}

QIcon TerminalProfile::previewIcon() const
{
    QFont font("monospace", 10);
    QFontMetrics fm(font);
    auto cw = fm.averageCharWidth();
    auto ch = fm.height();
    int y = 0;
    QPixmap pm(20*cw, 10*ch);
    QPainter p(&pm);
    p.fillRect(QRect(0,0,40*cw,20*ch), m_bgcolor);
    p.setPen(m_textcolor);
    p.drawText(QPoint(0,y), "darkstar [");
    p.setPen(m_boldcolor);
    p.drawText(QPoint(cw*10,y), "~/build-system");
    p.setPen(m_textcolor);

    y = y+ch+1;
    p.drawText(QPoint(0,y), "darkstar [");
    p.drawText(QPoint(cw*10,y), "~/build-system");

    return QIcon(pm.scaled(64,64));
}

QString TerminalProfile::profileName() const
{
    return m_profilename;
}

void TerminalProfile::setProfileName(const QString &name)
{
    m_profilename = name;
    emit profileNameChanged(name);
    emit profileChanged();
}

void TerminalProfile::setWindowTitle(const QString &title)
{
    m_windowtitle = title;
    emit windowTitleChanged(title);
    emit profileChanged();
}

QString TerminalProfile::windowTitle() const
{
    return m_windowtitle;
}


void TerminalProfile::setWindowTitleFlags(const WindowTitleFlags &flags)
{
    m_wndflags = flags;
    emit windowTitleFlagsChanged(flags);
    emit profileChanged();
}

TerminalProfile::WindowTitleFlags TerminalProfile::windowTitleFlags() const
{
    return m_wndflags;
}

void TerminalProfile::setTabTitleFlags(const TabTitleFlags &flags)
{
    m_tabflags = flags;
    emit tabTitleFlagsChanged(flags);
    emit profileChanged();
}

TerminalProfile::TabTitleFlags TerminalProfile::tabTitleFlags() const
{
    return m_tabflags;
}

void TerminalProfile::setFont(const QFont &font)
{
    m_font = font;
    emit fontChanged(font);
    emit profileChanged();
}

QFont TerminalProfile::font() const
{
    return m_font;
}

void TerminalProfile::setUseBold(const bool &bold)
{
    m_bold = bold;
    emit useBoldChanged(bold);
    emit profileChanged();
}

bool TerminalProfile::useBold() const
{
    return m_bold;
}

void TerminalProfile::setTextBlink(const bool &blink)
{
    m_blinkText = blink;
    emit textBlinkChanged(blink);
    emit profileChanged();
}

bool TerminalProfile::textBlink() const
{
    return m_blinkText;
}


void TerminalProfile::setUseANSIColor(const bool &color)
{
    m_ansicolor = color;
    emit useANSIColorChanged(color);
    emit profileChanged();
}

bool TerminalProfile::useANSIColor() const
{
    return m_ansicolor;
}

void TerminalProfile::setTextColor(const QColor &color)
{
    m_textcolor = color;
    emit textColorChanged(color);
    emit profileChanged();
}

QColor TerminalProfile::textColor() const
{
    return m_textcolor;
}

void TerminalProfile::setBoldColor(const QColor &color)
{
    m_boldcolor = color;
    emit boldColorChanged(color);
    emit profileChanged();
}

QColor TerminalProfile::boldColor() const
{
    return m_boldcolor;
}

void TerminalProfile::setSelectionColor(const QColor &color)
{
    m_selcolor = color;
    emit selectionColorChanged(color);
    emit profileChanged();
}

QColor TerminalProfile::selectionColor() const
{
    return m_selcolor;
}

void TerminalProfile::setBackgroundColor(const QColor &color)
{
    m_bgcolor = color;
    emit backgroundColorChanged(color);
    emit profileChanged();
}

QColor TerminalProfile::backgroundColor() const
{
    return m_bgcolor;
}

void TerminalProfile::setBackgroundImage(const QString &file)
{
    m_bgimg = file;
    emit backgroundImageChanged(file);
    emit profileChanged();
}

QString TerminalProfile::backgroundImage() const
{
    return m_bgimg;
}

void TerminalProfile::setBackgroundTransparency(const qreal &trans)
{
    m_bgtrans = trans;
    emit backgroundTransparencyChanged(trans);
    emit profileChanged();
}

qreal TerminalProfile::backgroundTransparency() const
{
    return m_bgtrans;
}

void TerminalProfile::setCursorType(const CursorType &type)
{
    m_cursor = type;
    emit cursorTypeChanged(type);
    emit profileChanged();
}

TerminalProfile::CursorType TerminalProfile::cursorType() const
{
    return m_cursor;
}

void TerminalProfile::setCursorColor(const QColor &color)
{
    m_cursorcolor = color;
    emit cursorColorChanged(color);
    emit profileChanged();
}

QColor TerminalProfile::cursorColor() const
{
    return m_cursorcolor;
}

QColor TerminalProfile::ansiColor(ANSIColors color) const
{
    return m_ansi[color];
}

void TerminalProfile::setCursorBlink(const bool &blink)
{
    m_blinkCursor = blink;
    emit cursorBlinkChanged(blink);
    emit profileChanged();
}

bool TerminalProfile::cursorBlink() const
{
    return m_blinkCursor;
}

void TerminalProfile::setStartupBehavior(const StartupBehavior &behavior)
{
    m_startup = behavior;
    emit startupBehaviorChanged(behavior);
    emit profileChanged();
}

TerminalProfile::StartupBehavior TerminalProfile::startupBehavior() const
{
    return m_startup;
}

void TerminalProfile::setAlternativeShell(const QString &shell)
{
    m_altshell = shell;
    Q_EMIT profileChanged();
}

QString TerminalProfile::alternativeShell() const
{
    return m_altshell;
}

void TerminalProfile::setCustomCommand(const QString &cmd)
{
    m_customcommand = cmd;
    Q_EMIT profileChanged();
}

void TerminalProfile::setANSIColor(const ANSIColors &ansi, const QColor &color)
{
    m_ansi[ansi] = color;
    Q_EMIT profileChanged();
    Q_EMIT ANSIColorChanged(ansi,color);
}

QString TerminalProfile::customCommand() const
{
    return m_customcommand;
}

void TerminalProfile::setTabCloseBehavior(const TabCloseBehavior &behavior)
{
    m_close = behavior;
    emit tabCloseBehaviorChanged(behavior);
    emit profileChanged();
}

TerminalProfile::TabCloseBehavior TerminalProfile::tabCloseBehavior() const
{
    return m_close;
}

void TerminalProfile::setCloseConsentBehavior(const CloseConsentBehavior &behavior)
{
    m_ask = behavior;
    emit closeConsentBehaviorChanged(behavior);
    emit profileChanged();
}

TerminalProfile::CloseConsentBehavior TerminalProfile::closeConsentBehavior() const
{
    return m_ask;
}

void TerminalProfile::setCloseConsentExceptions(const QStringList &exceptions)
{
    m_askexceptions = exceptions;
    emit closeConsentExceptionsChanged(m_askexceptions);
    emit profileChanged();
}

QStringList TerminalProfile::closeConsentExceptions() const
{
    return m_askexceptions;
}

void TerminalProfile::setBellSound(const AudibleBellSettings &bell)
{
    m_bellsound = bell;
    emit bellSoundChanged(bell);
    emit profileChanged();
}

TerminalProfile::AudibleBellSettings TerminalProfile::bellSound() const
{
    return m_bellsound;
}

void TerminalProfile::setBellFlags(const BellFlags &bell)
{
    m_bellflags = bell;
    emit bellFlagsChanged(bell);
    emit profileChanged();
}

TerminalProfile::BellFlags TerminalProfile::bellFlags() const
{
    return m_bellflags;
}

void TerminalProfile::setCustomBellSound(const QString &file)
{
    m_bellsoundfile = file;
    emit customBellSoundChanged(file);
    emit profileChanged();
}

QString TerminalProfile::customBellSound() const
{
    return m_bellsoundfile;
}

void TerminalProfile::setTerminfo(const QString &terminfo)
{
    m_terminfo = terminfo;
    emit terminfoChanged(terminfo);
    emit profileChanged();
}

QString TerminalProfile::terminfo() const
{
    return m_terminfo;
}

TerminalProfile::InputFlags TerminalProfile::inputFlags() const
{
    return m_inputflags;
}

void TerminalProfile::setInputFlags(const TerminalProfile::InputFlags &flags)
{
    m_inputflags = flags;
    emit inputFlagsChanged(flags);
    emit profileChanged();
}
