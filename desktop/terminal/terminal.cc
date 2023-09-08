#include "terminal.h"
#include "profile.h"
#include "application.h"

#if QT_VERSION >= 0x060000
#include <qtermwidget6/qtermwidget.h>
#else
#include <qtermwidget5/qtermwidget.h>
#endif

#include <QProcess>
#include <QDir>
#include <QDebug>

TerminalHost::TerminalHost(TerminalProfile *profile, QWidget *parent)
    : QObject(parent),
      m_widget(new QTermWidget(0)),
      m_profile(profile)
{
    srand(time(nullptr));
    m_id  = QByteArray(".")
            .prepend(QVariant(rand()).toByteArray())
            .append(QVariant((uint)time(nullptr)).toByteArray());

    qputenv("TERM", m_profile->terminfo().toUtf8());
    profileChanged();
    if(profile->startupBehavior() == TerminalProfile::CustomShell)
        m_widget->setShellProgram(m_profile->alternativeShell());

    m_widget->setScrollBarPosition(QTermWidget::ScrollBarRight);
    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_widget->setTrimPastedTrailingNewlines(true);
    m_widget->setTerminalSizeHint(true);
    connect(m_profile, &TerminalProfile::profileChanged, this, &TerminalHost::profileChanged);
    connect(m_widget, &QTermWidget::finished, this, &TerminalHost::terminalFinished);
    connect(m_widget, &QTermWidget::titleChanged, this, &TerminalHost::terminalTitleChanged);
    connect(m_widget, &QTermWidget::copyAvailable, this, &TerminalHost::copyAvailable);
    m_widget->startShellProgram();
}

TerminalHost::~TerminalHost()
{
    disconnect(m_profile, &TerminalProfile::profileChanged, this, &TerminalHost::profileChanged);
}

void TerminalHost::terminalTitleChanged()
{
    qDebug() << m_widget->title();
    emit windowTitleChanged(generateWindowTitle());
    emit tabTitleChanged(generateTabTitle());
}

void TerminalHost::profileChanged()
{
    m_widget->setTerminalOpacity(m_profile->backgroundTransparency());
    m_widget->setTerminalFont(m_profile->font());
    m_widget->setBoldIntense(m_profile->useBold());

    m_widget->setKeyboardCursorShape((QTermWidget::KeyboardCursorShape)m_profile->cursorType());
    m_widget->setBlinkingCursor(m_profile->cursorBlink());
    m_widget->setForegroundColor(m_profile->textColor());
    m_widget->setBackgroundColor(m_profile->backgroundColor());

    m_widget->setANSIColor((int)TerminalProfile::Black, m_profile->ansiColor(TerminalProfile::Black));
    m_widget->setANSIColor((int)TerminalProfile::Red, m_profile->ansiColor(TerminalProfile::Red));
    m_widget->setANSIColor((int)TerminalProfile::Green, m_profile->ansiColor(TerminalProfile::Green));
    m_widget->setANSIColor((int)TerminalProfile::Yellow, m_profile->ansiColor(TerminalProfile::Yellow));
    m_widget->setANSIColor((int)TerminalProfile::Blue, m_profile->ansiColor(TerminalProfile::Blue));
    m_widget->setANSIColor((int)TerminalProfile::Magenta, m_profile->ansiColor(TerminalProfile::Magenta));
    m_widget->setANSIColor((int)TerminalProfile::Cyan, m_profile->ansiColor(TerminalProfile::Cyan));
    m_widget->setANSIColor((int)TerminalProfile::White, m_profile->ansiColor(TerminalProfile::White));

    m_widget->setANSIColor((int)TerminalProfile::BrightBlack, m_profile->ansiColor(TerminalProfile::BrightBlack));
    m_widget->setANSIColor((int)TerminalProfile::BrightRed, m_profile->ansiColor(TerminalProfile::BrightRed));
    m_widget->setANSIColor((int)TerminalProfile::BrightGreen, m_profile->ansiColor(TerminalProfile::BrightGreen));
    m_widget->setANSIColor((int)TerminalProfile::BrightYellow, m_profile->ansiColor(TerminalProfile::BrightYellow));
    m_widget->setANSIColor((int)TerminalProfile::BrightBlue, m_profile->ansiColor(TerminalProfile::BrightBlue));
    m_widget->setANSIColor((int)TerminalProfile::BrightMagenta, m_profile->ansiColor(TerminalProfile::BrightMagenta));
    m_widget->setANSIColor((int)TerminalProfile::BrightCyan, m_profile->ansiColor(TerminalProfile::BrightCyan));
    m_widget->setANSIColor((int)TerminalProfile::BrightWhite, m_profile->ansiColor(TerminalProfile::BrightWhite));


    auto fn = m_profile->backgroundImage();
    QFile file(fn);
    if(file.open(QFile::ReadOnly))
    {
        file.close();
        m_widget->setTerminalBackgroundImage(fn);
    }
    else
         m_widget->setTerminalBackgroundImage("");

    //auto inputFlags = m_profile->inputFlags();
    //if(inputFlags & TerminalProfile::ScrollToBottom)
    //     m_widget->set

    switch(m_profile->tabCloseBehavior())
    {
    case TerminalProfile::CloseAlways:
    case TerminalProfile::CloseIfClean:
        m_widget->setAutoClose(true);
        break;
    case TerminalProfile::DontClose:
    default:
        m_widget->setAutoClose(false);
    }
}

void TerminalHost::terminalFinished()
{
    switch(m_profile->tabCloseBehavior())
    {
    case TerminalProfile::CloseAlways:
        qDebug() << "terminalFinished requestClose()";
        Q_EMIT requestClose();
        break;
    case TerminalProfile::CloseIfClean:
        // TODO: expanad qtermwidget to get status of shell proc
        qDebug() << "terminalFinished requestClose()";
        Q_EMIT requestClose();
        break;
    default:
        break;
    }
}

QString TerminalHost::getCwd()
{
    QDir dir(QString("/proc/%1/cwd").arg(m_widget->getShellPID()));
    if(dir.exists())
        return dir.absolutePath();

    return QString();
}

QString TerminalHost::getProcessLoop(uint pid)
{
    QDir dir(QString("/proc/%1/task/").arg(pid));
    if(dir.exists())
    {
        // shell is running
        auto tids = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        bool haschild = false;
        uint lastchild = 0;
        for(auto tid : tids)
        {
            QFile file(QString("/proc/%1/task/%2/children").arg(m_widget->getShellPID()).arg(tid));
            if(file.open(QFile::ReadOnly))
            {
                auto cpids = file.readAll().split(' ');
                file.close();
                for(auto cpid : cpids)
                {
                    if(QFile::exists(QString("/proc/%1/stat").arg(QString(cpid))))
                    {
                        haschild = true;
                        lastchild = cpid.toUInt();
                    }
                }

            }
        }

        if(haschild)
            return getProcessLoop(lastchild);
        else
            return getProcessName(pid);
    }
    return QString(tr("Invalid"));
}

QString TerminalHost::getProcessName(uint pid)
{
    QFile file(QString("/proc/%1/stat").arg(pid));
    if(file.open(QFile::ReadOnly))
    {
        auto proc = file.readAll().split(' ')[1];
        // remove the parentheses
        proc.remove(0,1);
        proc.remove(proc.length()-1,1);
        return proc;
    }
    return QString();
}

void TerminalHost::copyAvailable(bool canCopy)
{
    qDebug() << "canCopy";
    m_copy = canCopy;
    emit canCopyChanged(m_copy);
}

TerminalHost *TerminalHost::create(const QString &profile, QWidget *parent)
{
    auto pi = TerminalApplication::instance()->profileByName(profile);
    if(!pi)
        return nullptr;

    return new TerminalHost(pi, parent);
}

QByteArray TerminalHost::id()
{
    return m_id;
}

QTermWidget *TerminalHost::widget()
{
    return m_widget;
}

TerminalProfile *TerminalHost::profile()
{
    return m_profile;
}

bool TerminalHost::canClose()
{
    if(m_profile->closeConsentBehavior() == TerminalProfile::AskAlways)
        return false;

    if(m_profile->closeConsentBehavior() == TerminalProfile::AskNever)
        return true;

    return !hasBlockingTasks();
}

bool TerminalHost::hasBlockingTasks()
{
    QDir dir(QString("/proc/%1/task/").arg(m_widget->getShellPID()));
    if(dir.exists())
    {
        // shell is running
        auto tids = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(auto tid : tids)
        {
            QFile file(QString("/proc/%1/task/%2/children").arg(m_widget->getShellPID()).arg(tid));
            if(file.open(QFile::ReadOnly))
            {
                auto cpids = file.readAll().split(' ');
                file.close();
                for(auto cpid : cpids)
                {
                    if(QFile::exists(QString("/proc/%1/stat").arg(QString(cpid))))
                    {
                        QFile procstat(QString("/proc/%1/stat").arg(QString(cpid)));
                        file.open(QFile::ReadOnly);
                        auto statdata = file.readAll();
                        file.close();
                        if(statdata.isEmpty())
                            continue;

                        auto split = statdata.split(' ');
                        file.close();
                        auto exe = split[1].remove(0,1);
                        exe = exe.remove(exe.length()-1,1);
                        if(!m_profile->closeConsentExceptions().contains(exe))
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

QString TerminalHost::generateTabTitle()
{
    auto title = m_widget->title();
    if(title.isEmpty())
        title = m_profile->windowTitle();

    auto tflags = m_profile->tabTitleFlags();
    auto windowtitle = QStringList();
    auto tabtitle = QStringList();
    auto currentcwd = QString();
    if(tflags & TerminalProfile::WorkingDirDocumentTab ||
        tflags & TerminalProfile::WorkingDirDocumentPathTab)
        currentcwd = getCwd();

    auto procname = QString();
    if(tflags & TerminalProfile::ActiveProcessNameTab)
        procname = getProcessLoop(m_widget->getShellPID());

    // figure out our tab title
    if(tflags & TerminalProfile::WorkingDirDocumentTab)
    {
        if(tflags & TerminalProfile::WorkingDirDocumentPathTab)
            tabtitle.append(currentcwd);
        else
        {
            if(currentcwd == QLatin1String("/"))
                tabtitle.append("/");
            else
                tabtitle.append(currentcwd.split('/').last());
        }
    }

    return tabtitle.join('-');
}

QString TerminalHost::generateWindowTitle()
{
    auto title = m_widget->title();
    if(title.isEmpty())
        title = m_profile->windowTitle();

    auto wflags = m_profile->windowTitleFlags();
    auto wndtitle = QStringList();
    auto currentcwd = QString();
    if(wflags & TerminalProfile::WorkingDirDocument ||
        wflags & TerminalProfile::WorkingDirDocumentPath)
        currentcwd = getCwd();

    auto procname = QString();
    if(wflags & TerminalProfile::ActiveProcessName)
    {
        procname = getProcessLoop(m_widget->getShellPID());
        wndtitle.append(procname);
    }
    // figure out our tab title
    if(wflags & TerminalProfile::WorkingDirDocument)
    {
        if(wflags & TerminalProfile::WorkingDirDocumentPath)
            wndtitle.append(currentcwd);
        else
        {
            if(currentcwd == QLatin1String("/"))
                wndtitle.append("/");
            else
                wndtitle.append(currentcwd.split('/').last());
        }
    }

    return wndtitle.join('-');
}

void TerminalHost::setProfile(TerminalProfile *profile)
{
    Q_ASSERT(profile);
    m_profile = profile;
    profileChanged();
}

bool TerminalHost::hasSelection()
{
    int startCol,startRow,endCol,endRow;

    m_widget->getSelectionStart(startRow,startCol);
    m_widget->getSelectionEnd(endRow,endCol);

    if(startRow == endRow && startCol == endCol)
        return false;

    return true;
}
