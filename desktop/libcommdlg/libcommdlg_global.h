#ifndef LIBCOMMDLG_GLOBAL_H
#define LIBCOMMDLG_GLOBAL_H

#include <QtCore/qglobal.h>


#if defined(LIBCOMMDLG_LIBRARY)
#  define LIBCOMMDLG_EXPORT Q_DECL_EXPORT
#else
#  define LIBCOMMDLG_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBCOMMDLG_GLOBAL_H
