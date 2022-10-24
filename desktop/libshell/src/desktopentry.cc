/* This implementation was based upon libqtxdg */

#include "desktopentry.h"
#include "directories.h"

static const QLatin1String onlyShowInKey("OnlyShowIn");
static const QLatin1String notShowInKey("NotShowIn");
static const QLatin1String categoriesKey("Categories");
static const QLatin1String actionsKey("Actions");
static const QLatin1String extendPrefixKey("X-");
static const QLatin1String mimeTypeKey("MimeType");
static const QLatin1String applicationsStr("applications");

static const QLatin1String nameKey("Name");
static const QLatin1String typeKey("Type");
static const QLatin1String ApplicationStr("Application");
static const QLatin1String LinkStr("Link");
static const QLatin1String DirectoryStr("Directory");
static const QLatin1String execKey("Exec");
static const QLatin1String urlKey("URL");
static const QLatin1String iconKey("Icon");

class LSDesktopEntryPrivate : public QSharedData
{
public:
    LSDesktopEntryPrivate();

    inline void clear()
    {
        m_fileName.clear();
        m_valid = false;
        m_validChecked = false;
        m_isShown.clear();
        m_items.clear();
        m_type = LSDesktopEntry::TYPE_UNKNOWN;
    }

    bool read(const QString &prefix);
    LSDesktopEntry::Type detectType(LSDesktopEntry *q) const;
    bool startApplicationDetached(const LSDesktopEntry *q, const QString &action, const QStringList &urls) const;
    bool startLinkDetached(const LSDesktopEntry *q) const;
    bool startByDBus(const QString &action, const QStringList& urls) const;
    QStringList getListValue(const LSDesktopEntry *q, const QString &key, bool tryExtendPrefix) const;
public:
    QString m_fileName;
    bool m_valid;
    mutable bool m_validChecked;
    mutable QHash<QString, bool> m_isShown;
    QMap<QString, QVariant> m_items;

    LSDesktopEntry::Type m_type;
};

LSDesktopEntryPrivate::LSDesktopEntryPrivate():
    m_fileName(),
    m_valid(false),
    m_validChecked(false),
    m_isShown(),
    m_items(),
    m_type(LSDesktopEntry::TYPE_UNKNOWN)
{
}

bool LSDesktopEntryPrivate::read(const QString &prefix)
{
    QFile file(m_fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString section;
    QTextStream stream(&file);
    bool prefixExists = false;
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        if (line.startsWith(QLatin1Char('#')))
            continue;

        if (line.startsWith(QLatin1Char('[')) && line.endsWith(QLatin1Char(']')))
        {
            section = line.mid(1, line.length()-2);
            if (section == prefix)
                prefixExists = true;

            continue;
        }

        QString key = line.section(QLatin1Char('='), 0, 0).trimmed();
        QString value = line.section(QLatin1Char('='), 1).trimmed();

        if (key.isEmpty())
            continue;

        m_items[section + QLatin1Char('/') + key] = QVariant(value);
    }

    m_valid = (prefix.isEmpty()) || prefixExists;
    return m_valid;
}

LSDesktopEntry::Type LSDesktopEntryPrivate::detectType(LSDesktopEntry *q) const
{
    QString typeStr = q->value(typeKey).toString();
    if (typeStr == ApplicationStr)
        return LSDesktopEntry::TYPE_APP;

    if (typeStr == LinkStr)
        return LSDesktopEntry::TYPE_LINK;

    if (typeStr == DirectoryStr)
        return LSDesktopEntry::TYPE_DIR;

    if (!q->value(execKey).toString().isEmpty())
        return LSDesktopEntry::TYPE_APP;

    return LSDesktopEntry::TYPE_UNKNOWN;
}

LSDesktopEntry::LSDesktopEntry()
    :d(new LSDesktopEntryPrivate)
{

}

LSDesktopEntry::LSDesktopEntry(const LSDesktopEntry &copy)
    :d(copy.d)
{

}

LSDesktopEntry::~LSDesktopEntry() = default;

bool LSDesktopEntry::load(const QString& fileName)
{
    d->clear();
    if (fileName.startsWith(QDir::separator())) { // absolute path
        QFileInfo f(fileName);
        if (f.exists())
            d->m_fileName = f.canonicalFilePath();
        else
            return false;
    } else { // relative path
        const QString r = findDesktopFile(fileName);
        if (r.isEmpty())
            return false;
        else
            d->m_fileName = r;
    }
    d->read(prefix());
    d->m_valid = d->m_valid && check();
    d->m_type = d->detectType(this);
    return isValid();
}

bool LSDesktopEntry::save(QIODevice *device) const
{
    QTextStream stream(device);
    QMap<QString, QVariant>::const_iterator i = d->m_items.constBegin();

    QString section;
    while (i != d->m_items.constEnd())
    {
        QString path = i.key();
        QString sect =  path.section(QLatin1Char('/'),0,0);
        if (sect != section)
        {
            section = sect;
            stream << QLatin1Char('[') << section << QLatin1Char(']') << Qt::endl;
        }
        QString key = path.section(QLatin1Char('/'), 1);
        stream << key << QLatin1Char('=') << i.value().toString() << Qt::endl;
        ++i;
    }
    return true;
}

bool LSDesktopEntry::save(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    return save(&file);
}

QVariant LSDesktopEntry::value(const QString& key, const QVariant& defaultValue) const
{
    QString path = (!prefix().isEmpty()) ? prefix() + QLatin1Char('/') + key : key;
    QVariant res = d->m_items.value(path, defaultValue);
#if QT_VERSION >= 0x060000
    if (res.typeId() == QMetaType::QString)
#else
    if(res.type() == QVariant::String)
#endif
    {
        QString s = res.toString();
        return unEscape(s);
    }

    return res;
}

bool LSDesktopEntry::contains(const QString& key) const
{
    QString path = (!prefix().isEmpty()) ? prefix() + QLatin1Char('/') + key : key;
    return d->m_items.contains(path);
}

bool LSDesktopEntry::isValid() const { return d->m_valid; }

LSDesktopEntry::Type LSDesktopEntry::type() const
{
    return d->m_type;
}

QIcon LSDesktopEntry::icon() const
{
    QString di =
            value("Icon").toString();
    if(!di.isEmpty())
    {
        QIcon dico = QIcon::fromTheme(di);
        if(!dico.isNull())
            return dico;
    }

    return QIcon();
}

QString LSDesktopEntry::id(const QString &fileName, bool checkFileExists)
{
    const QFileInfo f(fileName);
    if (checkFileExists)
    {
        if (!f.exists())
            return QString();
    }

    auto id = f.absoluteFilePath();
    const QStringList dirs = QStringList() << LSDirectories::dataHome() << LSDirectories::dataDirs();

    for (auto d : dirs)
    {
        if (id.startsWith(d))
            id.replace(id.indexOf(d), d.size(), QString());
    }

    const QLatin1Char slash('/');
    const QString s = slash + applicationsStr + slash;
    if (!id.startsWith(s))
        return QString();

    id.replace(id.indexOf(s), s.size(), QString());
    id.replace(slash, QLatin1Char('-'));

    return id;
}

QString LSDesktopEntry::fileName() const { return d->m_fileName; }

void LSDesktopEntry::setValue(const QString &key, const QVariant &value)
{
    QString path = (!prefix().isEmpty()) ? prefix() + QLatin1Char('/') + key : key;
#if QT_VERSION >= 0x060000
    if (value.typeId() == QMetaType::QString)
#else
    if(value.type() == QVariant::String)
#endif
    {

        QString s=value.toString();
        if (key.toUpper() == QLatin1String("EXEC"))
            escapeExec(s);
        else
            escape(s);

        d->m_items[path] = QVariant(s);

        if (key.toUpper() == QLatin1String("TYPE"))
            d->m_type = d->detectType(this);
    }
    else
    {
        d->m_items[path] = value;
    }
}

QString LSDesktopEntry::localizedKey(const QString& key) const
{
    QString lang = QString::fromLocal8Bit(qgetenv("LC_MESSAGES"));

    if (lang.isEmpty())
        lang = QString::fromLocal8Bit(qgetenv("LC_ALL"));

    if (lang.isEmpty())
         lang = QString::fromLocal8Bit(qgetenv("LANG"));

    QString modifier = lang.section(QLatin1Char('@'), 1);
    if (!modifier.isEmpty())
        lang.truncate(lang.length() - modifier.length() - 1);

    QString encoding = lang.section(QLatin1Char('.'), 1);
    if (!encoding.isEmpty())
        lang.truncate(lang.length() - encoding.length() - 1);


    QString country = lang.section(QLatin1Char('_'), 1);
    if (!country.isEmpty())
        lang.truncate(lang.length() - country.length() - 1);

    //qDebug() << "LC_MESSAGES: " << qgetenv("LC_MESSAGES");
    //qDebug() << "Lang:" << lang;
    //qDebug() << "Country:" << country;
    //qDebug() << "Encoding:" << encoding;
    //qDebug() << "Modifier:" << modifier;


    if (!modifier.isEmpty() && !country.isEmpty())
    {
        QString k = QString::fromLatin1("%1[%2_%3@%4]").arg(key, lang, country, modifier);
        //qDebug() << "\t try " << k << contains(k);
        if (contains(k))
            return k;
    }

    if (!country.isEmpty())
    {
        QString k = QString::fromLatin1("%1[%2_%3]").arg(key, lang, country);
        //qDebug() << "\t try " << k  << contains(k);
        if (contains(k))
            return k;
    }

    if (!modifier.isEmpty())
    {
        QString k = QString::fromLatin1("%1[%2@%3]").arg(key, lang, modifier);
        //qDebug() << "\t try " << k  << contains(k);
        if (contains(k))
            return k;
    }

    QString k = QString::fromLatin1("%1[%2]").arg(key, lang);
    //qDebug() << "\t try " << k  << contains(k);
    if (contains(k))
        return k;


    //qDebug() << "\t try " << key  << contains(key);
    return key;
}


QVariant LSDesktopEntry::localizedValue(const QString& key, const QVariant& defaultValue) const
{
    return value(localizedKey(key), defaultValue);
}

void LSDesktopEntry::setLocalizedValue(const QString &key, const QVariant &value)
{
    setValue(localizedKey(key), value);
}

// STATIC FUNCTIONS !!!!

QString &LSDesktopEntry::doEscape(QString& str, const QHash<QChar,QChar> &repl)
{
    // First we replace slash.
    str.replace(QLatin1Char('\\'), QLatin1String("\\\\"));

    QHashIterator<QChar,QChar> i(repl);
    while (i.hasNext()) {
        i.next();
        if (i.key() != QLatin1Char('\\'))
            str.replace(i.key(), QString::fromLatin1("\\\\%1").arg(i.value()));
    }

    return str;
}

QString &LSDesktopEntry::escape(QString& str)
{
    QHash<QChar,QChar> repl;
    repl.insert(QLatin1Char('\n'),  QLatin1Char('n'));
    repl.insert(QLatin1Char('\t'),  QLatin1Char('t'));
    repl.insert(QLatin1Char('\r'),  QLatin1Char('r'));

    return doEscape(str, repl);
}

QString &LSDesktopEntry::escapeExec(QString& str)
{
    QHash<QChar,QChar> repl;
    // The parseCombinedArgString() splits the string by the space symbols,
    // we temporarily replace them on the special characters.
    // Replacement will reverse after the splitting.
    repl.insert(QLatin1Char('"'), QLatin1Char('"'));    // double quote,
    repl.insert(QLatin1Char('\''), QLatin1Char('\''));  // single quote ("'"),
    repl.insert(QLatin1Char('\\'), QLatin1Char('\\'));  // backslash character ("\"),
    repl.insert(QLatin1Char('$'), QLatin1Char('$'));    // dollar sign ("$"),

    return doEscape(str, repl);
}

QString &LSDesktopEntry::doUnEscape(QString& str, const QHash<QChar,QChar> &repl)
{
    int n = 0;
    while (true)
    {
        n=str.indexOf(QLatin1String("\\"), n);
        if (n < 0 || n > str.length() - 2)
            break;

        if (repl.contains(str.at(n+1)))
        {
            str.replace(n, 2, repl.value(str.at(n+1)));
        }

        n++;
    }

    return str;
}

QString &LSDesktopEntry::unEscape(QString& str)
{
    QHash<QChar,QChar> repl;
    repl.insert(QLatin1Char('\\'), QLatin1Char('\\'));
    repl.insert(QLatin1Char('s'),  QLatin1Char(' '));
    repl.insert(QLatin1Char('n'),  QLatin1Char('\n'));
    repl.insert(QLatin1Char('t'),  QLatin1Char('\t'));
    repl.insert(QLatin1Char('r'),  QLatin1Char('\r'));

    return doUnEscape(str, repl);
}


QString &LSDesktopEntry::unEscapeExec(QString& str)
{
    unEscape(str);
    QHash<QChar,QChar> repl;
    // The parseCombinedArgString() splits the string by the space symbols,
    // we temporarily replace them on the special characters.
    // Replacement will reverse after the splitting.
    repl.insert(QLatin1Char(' '),  QLatin1Char(01));    // space
    repl.insert(QLatin1Char('\t'), QLatin1Char(02));    // tab
    repl.insert(QLatin1Char('\n'), QLatin1Char(03));    // newline,

    repl.insert(QLatin1Char('"'), QLatin1Char('"'));    // double quote,
    repl.insert(QLatin1Char('\''), QLatin1Char('\''));  // single quote ("'"),
    repl.insert(QLatin1Char('\\'), QLatin1Char('\\'));  // backslash character ("\"),
    repl.insert(QLatin1Char('>'), QLatin1Char('>'));    // greater-than sign (">"),
    repl.insert(QLatin1Char('<'), QLatin1Char('<'));    // less-than sign ("<"),
    repl.insert(QLatin1Char('~'), QLatin1Char('~'));    // tilde ("~"),
    repl.insert(QLatin1Char('|'), QLatin1Char('|'));    // vertical bar ("|"),
    repl.insert(QLatin1Char('&'), QLatin1Char('&'));    // ampersand ("&"),
    repl.insert(QLatin1Char(';'), QLatin1Char(';'));    // semicolon (";"),
    repl.insert(QLatin1Char('$'), QLatin1Char('$'));    // dollar sign ("$"),
    repl.insert(QLatin1Char('*'), QLatin1Char('*'));    // asterisk ("*"),
    repl.insert(QLatin1Char('?'), QLatin1Char('?'));    // question mark ("?"),
    repl.insert(QLatin1Char('#'), QLatin1Char('#'));    // hash mark ("#"),
    repl.insert(QLatin1Char('('), QLatin1Char('('));    // parenthesis ("(")
    repl.insert(QLatin1Char(')'), QLatin1Char(')'));    // parenthesis (")")
    repl.insert(QLatin1Char('`'), QLatin1Char('`'));    // backtick character ("`").

    return doUnEscape(str, repl);
}

QString LSDesktopEntry::findDesktopFile(const QString& dirName, const QString& desktopName)
{
    QDir dir(dirName);
    QFileInfo fi(dir, desktopName);

    if (fi.exists())
        return fi.canonicalFilePath();

    // Working recursively ............
    const QFileInfoList dirs = dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &d : dirs)
    {
        QString cn = d.canonicalFilePath();
        if (dirName != cn)
        {
            QString f = findDesktopFile(cn, desktopName);
            if (!f.isEmpty())
                return f;
        }
    }

    return QString();
}


QString LSDesktopEntry::findDesktopFile(const QString& desktopName)
{
    QStringList dataDirs = LSDirectories::dataDirs();
    dataDirs.prepend(LSDirectories::dataHome(false));

    for (const QString &dirName : qAsConst(dataDirs))
    {
        QString f = findDesktopFile(dirName + QLatin1String("/applications"), desktopName);
        if (!f.isEmpty())
            return f;
    }

    return QString();
}
