#include "directories.h"

static const QString userDirectoryString[8] =
{
    QLatin1String("Desktop"),
    QLatin1String("Downloads"),
    QLatin1String("Templates"),
    QLatin1String("Publicshare"),
    QLatin1String("Documents"),
    QLatin1String("Music"),
    QLatin1String("Pictures"),
    QLatin1String("Videos")
};

QString LSDirectories::userDirDefault(LSDirectories::UserDirectory dir)
{
    // possible values for UserDirectory
    Q_ASSERT(!(dir < LSDirectories::Desktop || dir > LSDirectories::Videos));
    if (dir < LSDirectories::Desktop || dir > LSDirectories::Videos)
        return QString();

    return userDirFallback(dir);
}

QString LSDirectories::userDir(LSDirectories::UserDirectory dir)
{
    // possible values for UserDirectory
    Q_ASSERT(!(dir < LSDirectories::Desktop || dir > LSDirectories::Videos));
    if (dir < LSDirectories::Desktop || dir > LSDirectories::Videos)
        return QString();

    QString folderName = userDirectoryString[dir];

    const QString fallback = userDirFallback(dir);

    QString configDir(configHome());
    QFile configFile(configDir + QLatin1String("/user-dirs.dirs"));
    if (!configFile.exists())
        return fallback;

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return fallback;

    QString userDirVar(QLatin1String("XDG_") + folderName.toUpper() + QLatin1String("_DIR"));
    QTextStream in(&configFile);
    QString line;
    while (!in.atEnd())
    {
        line = in.readLine();
        if (line.contains(userDirVar))
        {
            configFile.close();

            // get path between quotes
            line = line.section(QLatin1Char('"'), 1, 1);
            if (line.isEmpty())
                return fallback;
            line.replace(QLatin1String("$HOME"), QLatin1String("~"));
            fixShellShortcuts(line);
            return line;
        }
    }

    configFile.close();
    return fallback;
}

bool LSDirectories::setUserDir(LSDirectories::UserDirectory dir, const QString& value, bool createDir)
{
    // possible values for UserDirectory
    Q_ASSERT(!(dir < LSDirectories::Desktop || dir > LSDirectories::Videos));
    if (dir < LSDirectories::Desktop || dir > LSDirectories::Videos)
        return false;

    const QString home = QFile::decodeName(qgetenv("HOME"));
    if (!(value.startsWith(QLatin1String("$HOME"))
                           || value.startsWith(QLatin1String("~/"))
                           || value.startsWith(home)
                           || value.startsWith(QDir(home).canonicalPath())))
    return false;

    QString folderName = userDirectoryString[dir];

    QString configDir(configHome());
    QFile configFile(configDir + QLatin1String("/user-dirs.dirs"));

    // create the file if doesn't exist and opens it
    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return false;

    QTextStream stream(&configFile);
    QVector<QString> lines;
    QString line;
    bool foundVar = false;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        if (line.indexOf(QLatin1String("XDG_") + folderName.toUpper() + QLatin1String("_DIR")) == 0)
        {
            foundVar = true;
            QString path = line.section(QLatin1Char('"'), 1, 1);
            line.replace(path, value);
            lines.append(line);
        }
        else if (line.indexOf(QLatin1String("XDG_")) == 0)
        {
            lines.append(line);
        }
    }

    stream.reset();
    configFile.resize(0);
    if (!foundVar)
        stream << QString::fromLatin1("XDG_%1_DIR=\"%2\"\n").arg(folderName.toUpper(),(value));

    for (QVector<QString>::iterator i = lines.begin(); i != lines.end(); ++i)
        stream << *i << QLatin1Char('\n');

    configFile.close();

    if (createDir) {
        QString path = QString(value).replace(QLatin1String("$HOME"), QLatin1String("~"));
        fixShellShortcuts(path);
        QDir().mkpath(path);
    }

    return true;
}

QString LSDirectories::dataHome(bool createDir)
{
    QString s = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    fixShellShortcuts(s);
    if (createDir)
        return createDirectory(s);

   removeEndingSlash(s);
   return s;
}

QString LSDirectories::configHome(bool createDir)
{
    QString s = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    fixShellShortcuts(s);
    if (createDir)
        return createDirectory(s);

   removeEndingSlash(s);
   return s;
}

QStringList LSDirectories::dataDirs(const QString &postfix)
{
    QString d = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));
    QStringList dirs = d.split(QLatin1Char(':'), Qt::SkipEmptyParts);

    if (dirs.isEmpty()) {
        dirs.append(QString::fromLatin1("/usr/local/share"));
        dirs.append(QString::fromLatin1("/usr/share"));
    } else {
        QMutableListIterator<QString> it(dirs);
        while (it.hasNext()) {
            const QString dir = it.next();
            if (!dir.startsWith(QLatin1Char('/')))
                it.remove();
        }
    }

    dirs.removeDuplicates();
    cleanAndAddPostfix(dirs, postfix);
    return dirs;

}

QStringList LSDirectories::configDirs(const QString &postfix)
{
    QStringList dirs;
    const QString env = QFile::decodeName(qgetenv("XDG_CONFIG_DIRS"));
    if (env.isEmpty())
        dirs.append(QString::fromLatin1("/etc/xdg"));
    else
        dirs = env.split(QLatin1Char(':'), Qt::SkipEmptyParts);

    cleanAndAddPostfix(dirs, postfix);
    return dirs;
}

QString LSDirectories::cacheHome(bool createDir)
{
    QString s = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    fixShellShortcuts(s);
    if (createDir)
        return createDirectory(s);

    removeEndingSlash(s);
    return s;
}

QString LSDirectories::runtimeDir()
{
    QString result = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    fixShellShortcuts(result);
    removeEndingSlash(result);
    return result;
}

QString LSDirectories::autostartHome(bool createDir)
{
    QString s = QString::fromLatin1("%1/autostart").arg(configHome(createDir));
    fixShellShortcuts(s);

    if (createDir)
        return createDirectory(s);

     QDir d(s);
     QString r = d.absolutePath();
     removeEndingSlash(r);
     return r;
}

QStringList LSDirectories::autostartDirs(const QString &postfix)
{
    QStringList dirs;
    const QStringList s = configDirs();
    for (const QString &dir : s)
        dirs << QString::fromLatin1("%1/autostart").arg(dir) + postfix;

    return dirs;
}

// Static Helper Functions
void LSDirectories::fixShellShortcuts(QString &s)
{
    if (s.startsWith(QLatin1Char('~')))
        s = QFile::decodeName(qgetenv("HOME")) + (s).mid(1);
}

void LSDirectories::removeEndingSlash(QString &s)
{
    // We don't check for empty strings. Caller must check it.

    // Remove the ending slash, except for root dirs.
    if (s.length() > 1 && s.endsWith(QLatin1Char('/')))
        s.chop(1);
}

QString LSDirectories::createDirectory(const QString &dir)
{
    QDir d(dir);
    if (!d.exists())
    {
        if (!d.mkpath(QLatin1String(".")))
        {
            qWarning() << QString::fromLatin1("Can't create %1 directory.").arg(d.absolutePath());
        }
    }
    QString r = d.absolutePath();
    removeEndingSlash(r);
    return r;
}

void LSDirectories::cleanAndAddPostfix(QStringList &dirs, const QString& postfix)
{
    const int N = dirs.count();
    for(int i = 0; i < N; ++i)
    {
        fixShellShortcuts(dirs[i]);
        removeEndingSlash(dirs[i]);
        dirs[i].append(postfix);
    }
}

QString LSDirectories::userDirFallback(LSDirectories::UserDirectory dir)
{
    QString fallback;
    const QString home = QFile::decodeName(qgetenv("HOME"));

    if (home.isEmpty())
        return QString::fromLatin1("/tmp");
    else if (dir == LSDirectories::Desktop)
        fallback = QString::fromLatin1("%1/%2").arg(home, QLatin1String("Desktop"));
    else
        fallback = home;

    return fallback;
}
