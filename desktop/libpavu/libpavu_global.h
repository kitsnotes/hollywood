#ifndef LIBPAVU_GLOBAL_H
#define LIBPAVU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBPAVU_LIBRARY)
#  define LIBPAVU_EXPORT Q_DECL_EXPORT
#else
#  define LIBPAVU_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBPAVU_GLOBAL_H
