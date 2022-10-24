#ifndef LIBCOMPOSITOR_GLOBAL_H
#define LIBCOMPOSITOR_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtGui/QWindow>
#include <QLoggingCategory>

#if defined(LIBCOMPOSITOR_LIBRARY)
#  define LIBCOMPOSITOR_EXPORT Q_DECL_EXPORT
#else
#  define LIBCOMPOSITOR_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBCOMPOSITOR_GLOBAL_H
