#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include "editor.h"

#if defined(Q_WS_MAC) || defined(Q_WS_WIN)
#include "../hunspell/src/hunspell/hunspell.hxx"
#else
#include <hunspell/hunspell.hxx>
#endif
#if QT_VERSION >= 0x060000
#include <QtCore5Compat/QTextCodec>
#endif
class Hunspell;
class SpellCheck : public QObject
{
    Q_OBJECT
public:
    explicit SpellCheck(QObject *parent = Q_NULLPTR);
    bool checkWord(const QString &word);
    QStringList suggestionsForWord(const QString &word);
    void addWordToIgnore(const QString &word);
    void addWordToPersonalDictionary(const QString &word);
    static QStringList availableDictionaries();

public slots:
    void updatePreferences();
private:
    void loadHunspell();
    void createPersonalDictionary();
private:
    Hunspell* m_hs;
    QString m_encoding;
    QTextCodec* m_codec;
    QString m_dictionary;
};

#endif // SPELLCHECK_H
