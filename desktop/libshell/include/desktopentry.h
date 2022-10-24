/* This implementation was based upon libqtxdg */

#ifndef LSDESKTOPENTRY_H
#define LSDESKTOPENTRY_H

#include "libshell_int.h"

#include <QSharedData>
#include <QSharedDataPointer>

class LSDesktopEntryPrivate;
class LIBSHELL_EXPORT LSDesktopEntry
{
public:
    enum Type {
        TYPE_UNKNOWN,
        TYPE_APP,
        TYPE_LINK,
        TYPE_DIR
    };
    LSDesktopEntry();
    LSDesktopEntry(const LSDesktopEntry &copy);
    virtual ~LSDesktopEntry();

    virtual bool load(const QString &file);
    virtual bool save(QIODevice *device) const;
    virtual bool save(const QString &file) const;

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    QVariant localizedValue(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);
    void setLocalizedValue(const QString &key, const QVariant &value);

    QString fileName() const;
    bool contains(const QString& key) const;
    bool isValid() const;
    Type type() const;

    QIcon icon() const;

    static QString id(const QString &fileName, bool checkFileExists = true);

    // static helper functions
    static QString findDesktopFile(const QString& dirName, const QString& desktopName);
    static QString findDesktopFile(const QString& desktopName);
    static QString &doEscape(QString& str, const QHash<QChar,QChar> &repl);
    static QString &doUnEscape(QString& str, const QHash<QChar,QChar> &repl);
    static QString &escape(QString& str);
    static QString &escapeExec(QString& str);
    static QString &unEscape(QString& str);
    static QString &unEscapeExec(QString& str);
signals:
protected:
    virtual QString prefix() const { return QLatin1String("Desktop Entry"); }
    virtual bool check() const { return true; }
private:
    QString localizedKey(const QString& key) const;
    QSharedDataPointer<LSDesktopEntryPrivate> d;
};

typedef QList<LSDesktopEntry> LSDesktopEntryList;

#endif // LSDESKTOPENTRY_H
