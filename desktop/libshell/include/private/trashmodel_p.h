#ifndef DESKTOPMODEL_P_H
#define DESKTOPMODEL_P_H

#include <QAbstractListModel>
#include <QFile>
#include <QDir>
#include <QFileIconProvider>
#include <QBasicTimer>
#include <QTimer>
#include <QCollator>
#include <QVector>
#include <QHash>
#include <QMimeDatabase>
#include <QFileSystemWatcher>
#include <QDateTime>

class LSFSThread;
class LSTrashModel;

class LSTrashModelItem
{
public:
    LSTrashModelItem(const QString &trashfile);
    QDir trashDir() const { return m_trashdir; }
    QString displayName() const { return m_origin.split('/').last(); }
    QString fileName() const { return m_name; }
    QString mimeType() const { return m_mime; }
    QString origin() const { return m_origin; }
    bool valid() const { return m_valid; }
    bool operator ==(const LSTrashModelItem &fileInfo) const;
private:
    friend class LSTrashModelPrivate;
    bool m_valid = false;
    QDir m_trashdir;
    QString m_name;
    QString m_mime;
    QDateTime m_trashtime;
    QString m_origin;
};

class LSTrashModelPrivate
{
private:
    friend class LSTrashModel;
    LSTrashModelPrivate(LSTrashModel *parent);
    ~LSTrashModelPrivate() = default;
    LSTrashModel *d;
    bool hasFile(const QString &file);
    int rowForFile(const QString &file);
private:
    QFileSystemWatcher *m_fileInfoGatherer;
    QList<LSTrashModelItem> m_items;
    QDir m_trashdir;
};

#endif // DESKTOPMODEL_P_H
