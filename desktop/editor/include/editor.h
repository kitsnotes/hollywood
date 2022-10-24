#ifndef EDITOR_H
#define EDITOR_H

// we store our global defines in a separate file
// so the windows resource compiler doesn't complain
#include "rcdefs.h"

#ifndef LUVEDIT_BUILD
#define LUVEDIT_BUILD "Unknown"
#endif

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

typedef QMap<time_t, QString> MRUFiles;

#define MAX_ARG_LEN             100

// the absolute upper bound of most recent files
#define MAX_MRU_LEN             40

#include <QtPrintSupport>

#ifdef Q_OS_MAC
//#include <QtMacExtras>
#include <CoreFoundation/CoreFoundation.h>
#include <QMacNativeWidget>
#include <QMacCocoaViewContainer>
#include "mac/qsearchfield.h"
#include "mac/qbutton.h"

#endif

#ifdef Q_OS_WIN
#include <windows.h>
#include <Dwmapi.h>
#include <VersionHelpers.h>
#include <QtWinExtras/QtWinExtras>
#endif

enum LineEnding
{
    Unix,
    Windows,
    Mac
};

enum LanguageFeatureType
{
    PlainText,
    CPP,
    HTML,
    XML
};

enum SpellCheckAction
{
    Abort,
    IgnoreWord,
    ChangeWord,
    LearnWord
};

struct FindSettings
{
    QString find_str;
    QString replace_str;
    bool regexp = false;
    bool whole_words = false;
    bool case_sensitive = false;
    bool wrap_around = false;
};

#include "application.h"
#include "document.h"

// main widget ui
#include "editorwindow.h"
#include "tabwidget.h"
#include "editorwidget.h"

#ifdef Q_OS_UNIX
#define LUVEDIT_DEFAULT_DICT "en_US"
#else
#define LUVEDIT_DEFAULT_DICT "US English"
#endif
#include "spellcheck.h"
#include "spellcheckdialog.h"

#ifdef Q_WS_MAC
#include "mac/mactoolbar.h"
#endif

// dialogs
#include "colordialog.h"
#include "preferencesdialog.h"
#include "finddialog.h"

// syntax highlighting
#include "features/abstractfeatures.h"
#include "features/plain/plainfeatures.h"
#include "features/cpp/cfeatures.h"
#include "features/html/htmlfeatures.h"

#endif // EDITOR_H
