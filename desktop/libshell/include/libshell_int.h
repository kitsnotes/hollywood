#ifndef LIBSHELL_INT_H
#define LIBSHELL_INT_H

#include <QDebug>

#include <QClipboard>
#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QScreen>
#include <QList>
#include <QKeySequence>
#include <QFileSystemModel>
#include <QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QColumnView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include <QFileIconProvider>
#include <QFileSystemWatcher>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QUuid>

#include <QPushButton>
#include <QFileSystemModel>
#include <QCompleter>
#include <QStackedLayout>
#include <QFileIconProvider>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QActionGroup>


namespace ArionShell
{
    enum HostMode {
        HOST_SHELL,
        HOST_OPEN_DIALOG,
        HOST_SAVE_DIALOG,
        HOST_FOLDER_DIALOG,
        HOST_EMBED
    };
    enum ViewMode {
        VIEW_ICONS,
        VIEW_LIST,
        VIEW_COLUMN
    };
    enum WindowMode {
        WINDOW_DESKTOP,
        WINDOW_BROWSER
    };
    enum ShellActions {
        ACT_FILE_NEW_TAB,
        ACT_FILE_NEW_FOLDER,
        ACT_FILE_NEW_FILE,
        ACT_FILE_GET_INFO,
        ACT_FILE_RENAME,
        ACT_FILE_ARCHIVE,
        ACT_FILE_TRASH,
        ACT_FILE_OPEN_WITH,
        ACT_EDIT_UNDO,
        ACT_EDIT_REDO,
        ACT_EDIT_CUT,
        ACT_EDIT_COPY,
        ACT_EDIT_PASTE,
        ACT_EDIT_SEL_ALL,
        ACT_EDIT_INV_SEL,
        ACT_VIEW_ICONS,
        ACT_VIEW_LIST,
        ACT_VIEW_COLUMNS,
        ACT_VIEW_OPTIONS,
        ACT_GO_BACK,
        ACT_GO_FORWARD,
        ACT_GO_ENCLOSING_FOLDER
    };
}

#define GLOBAL_APPS_FOLDER      "/usr/share/applications"
#define LOCAL_APPS_FOLDER       "~/.local/share/applications"
#define GLOBAL_MIMEDB_FOLDER    "/usr/share/mime"
#define LOCAL_MIMEDB_FOLDER     "~/.local/share/mime"
#define LOCAL_CONFIG_FOLDER     "~/.config"
#define MIMEINFO_CACHE          "mimeinfo.cache"
#define MIMEDB_CACHE            "mime.cache"
#define DEFAULTS_LIST           "mimeapps.list"

#if defined(LIBSHELL_LIBRARY)
#  define LIBSHELL_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHELL_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBSHELL_INT_H
