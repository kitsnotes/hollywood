// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include "libshell_int.h"

class LIBSHELL_EXPORT LSDirectories
{
public:
    enum UserDirectory
    {
        Desktop,
        Downloads,
        Templates,
        PublicShare,
        Documents,
        Music,
        Pictures,
        Videos
    };

    static QString userDir(UserDirectory dir);
    static QString userDirDefault(UserDirectory dir);
    static bool setUserDir(UserDirectory dir, const QString &value, bool createDir);
    static QString dataHome(bool createDir=true);
    static QString configHome(bool createDir=true);
    static QStringList dataDirs(const QString &postfix = QString());
    static QStringList configDirs(const QString &postfix = QString());
    static QString cacheHome(bool createDir=true);
    static QString runtimeDir();
    static QString autostartHome(bool createDir=true);
    static QStringList autostartDirs(const QString &postfix = QString());

    static void fixShellShortcuts(QString &s);
    static void removeEndingSlash(QString &s);
    static QString createDirectory(const QString &dir);

    static void cleanAndAddPostfix(QStringList &dirs, const QString& postfix);
    static QString userDirFallback(LSDirectories::UserDirectory dir);
};
