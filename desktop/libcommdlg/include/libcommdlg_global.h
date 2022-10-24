#ifndef LIBCOMMDLG_GLOBAL_H
#define LIBCOMMDLG_GLOBAL_H

#include <QtCore/qglobal.h>

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

#if defined(LIBCOMMDLG_LIBRARY)
#  define LIBCOMMDLG_EXPORT Q_DECL_EXPORT
#else
#  define LIBCOMMDLG_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBCOMMDLG_GLOBAL_H
