#include "spellcheck.h"

SpellCheck::SpellCheck(QObject *parent)
    : QObject(parent)
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    m_dictionary = settings.value("SpellingDictionary", LUVEDIT_DEFAULT_DICT).toString();

    createPersonalDictionary();
    loadHunspell();
}

void SpellCheck::loadHunspell()
{
#ifdef Q_WS_WIN
    QString path = QString("%1/Dictionaries").arg(QCoreApplication::applicationDirPath());
#endif

#ifdef Q_WS_MAC
    CFURLRef url = (CFURLRef)CFAutorelease((CFURLRef)CFBundleCopyBundleURL(CFBundleGetMainBundle()));
    QString path = QString("%1/Contents/Resources/Dictionaries").arg(QUrl::fromCFURL(url).path());
#endif

    QString personal = QString("%1/Personal Dictionaries/%2/personal.dic")
            .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
            .arg(m_dictionary);

#ifdef __linux__
    QString path = QString("/usr/share/hunspell");
    QString affPath = QString("%1/%2.aff").arg(path).arg(m_dictionary);
    QString dicPath = QString("%1/%2.dic").arg(path).arg(m_dictionary);
#else
    QString affPath = QString("%1/%2/%3").arg(path).arg(m_dictionary).arg("index.aff");
    QString dicPath = QString("%1/%2/%3").arg(path).arg(m_dictionary).arg("index.dic");
#endif
    m_hs = new Hunspell(affPath.toUtf8().constData(), dicPath.toUtf8().constData());
}

bool SpellCheck::checkWord(const QString &word)
{
    if(!m_hs)
        return true;

    return m_hs->spell(word.toUtf8().data());
}

QStringList SpellCheck::suggestionsForWord(const QString &word)
{
    if(!m_hs)
        return QStringList();

    char** lst;
    int sug = m_hs->suggest(&lst, word.toUtf8().data());

    QStringList suggestions;

    for(int i = 0; i < sug; ++i)
    {
        suggestions.append(QString(lst[i]));
        free(lst[i]);
    }

    return suggestions;
}

void SpellCheck::addWordToIgnore(const QString &word)
{
    m_hs->add(word.toUtf8().data());
}

void SpellCheck::addWordToPersonalDictionary(const QString &word)
{
    m_hs->add(word.toUtf8().data());
    QString personal = QString("%1/Personal Dictionaries/%2/personal.dic")
            .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
            .arg(m_dictionary);

    QFile file(personal);
    if(file.open(QIODevice::Append))
    {
        QTextStream s(&file);
        s << word << "\n";
        file.close();
    }
}

QStringList SpellCheck::availableDictionaries()
{
     QStringList available;
#ifdef WINDOWS_DESKTOP
    QString path = QString("%1/Dictionaries").arg(QCoreApplication::applicationDirPath());
#endif

#ifdef THINK_DIFFERENT
    CFURLRef url = (CFURLRef)CFAutorelease((CFURLRef)CFBundleCopyBundleURL(CFBundleGetMainBundle()));
    QString path = QString("%1/Contents/Resources/Dictionaries").arg(QUrl::fromCFURL(url).path());
#endif

#ifdef __linux__
    QString path = QString("/usr/share/hunspell");
    QDir dir(path);
    auto lst = dir.entryList(QStringList() << "*.dic", QDir::Files|QDir::NoDotAndDotDot);
    for(auto fn : lst)
    {
        auto file = fn.split('.');
        file.removeLast();
        auto finalName = file.join('.');
        bool hasAff = QFile::exists(QString("%1/%2.aff").arg(path)
                                    .arg(finalName));
        bool hasDic = QFile::exists(QString("%1/%2.dic").arg(path)
                                    .arg(finalName));

        if(hasAff && hasDic)
            available.append(finalName);
    }
#else
    QDir dir(path);
    QStringList lst = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
    foreach(QString dir, lst)
    {
        bool hasAff = QFile::exists(QString("%1/%2/%3").arg(path)
                                    .arg(dir).arg("index.aff"));
        bool hasDic = QFile::exists(QString("%1/%2/%3").arg(path)
                                    .arg(dir).arg("index.dic"));

        if(hasAff && hasDic)
            available.append(dir);
    }
#endif
    return available;
}

void SpellCheck::createPersonalDictionary()
{
    QString personal = QString("%1/Personal Dictionaries/%2/personal.dic")
            .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
            .arg(m_dictionary);

    if(QFile::exists(personal))
        return;

    QFile file(personal);
    if(file.open(QFile::WriteOnly))
    {
        file.write("");
        file.close();
        return;
    }

    // TODO: warn about this?
}

 void SpellCheck::updatePreferences()
 {
    // TODO: re-init with new lang dictionary
 }
