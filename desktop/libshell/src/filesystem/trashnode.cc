#include "trashnode.h"
#include <QDateTime>
#include <errno.h>
#include "hwfileiconprovider.h"
#include "trashmodel.h"

#include <QCoreApplication>
#include <QList>
#include <QString>
#include <QSettings>

LSTrashNode::LSTrashNode(const QString &filename, LSTrashNode *p)
    : fileName(filename), m_parent(p) {}

LSTrashNode::~LSTrashNode() {
    qDeleteAll(children);
}

qint64 LSTrashNode::size() const { return 0; }

QString LSTrashNode::type() const
{
    if(m_info.isDir())
        return QCoreApplication::tr("Folder");

    // TODO: mime type check
    return QCoreApplication::tr("File");

}

QDateTime LSTrashNode::deleteTime() const
{
    return m_deleteTime;
}


bool LSTrashNode::isDir() const
{
    return m_info.isDir();
}

QFileInfo LSTrashNode::fileInfo() const { return m_info; }

/* TODO: fix this */
bool LSTrashNode::caseSensitive() const { /* if (info) return info.data()->isCaseSensitive(); */ return true; }


QIcon LSTrashNode::icon() const
{
    HWFileIconProvider p;
    return p.icon(m_info);
}

QString LSTrashNode::originalPath() const
{
    return m_origPath;
}

void LSTrashNode::populate(QFileInfo &info)
{
    m_info = info;
    bool toplevel = false;
    auto tokens = m_info.absoluteFilePath().split('/');
    tokens.removeLast();
    auto parentdir = tokens.join('/');
    if(parentdir == LSTrashModel::xdgTrashDir()+"/files")
        toplevel = true;

    if(toplevel)
    {
        auto trashinfo = m_info.fileName()+".trashinfo";
        // see if we have a .trashinfo file
        QDir dir(LSTrashModel::xdgTrashDir()+"/info");
        if(dir.exists())
        {
            QSettings settings(dir.filePath(trashinfo), QSettings::IniFormat);
            settings.beginGroup("Trash Info");
            m_origPath = settings.value("Path").toString();
            auto deldate = settings.value("DeletionDate").toString();
            m_deleteTime = QDateTime::fromString(deldate, "yyyy-MM-ddThh:mm:ss");
        }
    }
}

bool LSTrashNode::operator >(const QString &name) const
{
    if (caseSensitive())
        return fileName > name;
    return QString::compare(fileName, name, Qt::CaseInsensitive) > 0;
}

bool LSTrashNode::operator <(const QString &name) const {
    if (caseSensitive())
        return fileName < name;
    return QString::compare(fileName, name, Qt::CaseInsensitive) < 0;
}

bool LSTrashNode::operator !=(const QFileInfo &fileInfo) const {
    return !operator==(fileInfo);
}

bool LSTrashNode::operator ==(const QString &name) const
{
    if (caseSensitive())
        return fileName == name;
    return QString::compare(fileName, name, Qt::CaseInsensitive) == 0;
}


int LSTrashNode::visibleLocation(const QString &childName)
{
    return visibleChildren.indexOf(childName);
}

bool LSTrashNode::operator ==(const QFileInfo &fileInfo) const
{
    return QFileInfo(fileName) == fileInfo;
}

bool LSTrashNode::operator <(const LSTrashNode &node) const
{
    if (caseSensitive() || node.caseSensitive())
        return fileName < node.fileName;
    return QString::compare(fileName, node.fileName, Qt::CaseInsensitive) < 0;
}

void LSTrashNode::updateIcon(QFileIconProvider *iconProvider, const QString &path)
{
    for (LSTrashNode *child : qAsConst(children))
    {
        if (!path.isEmpty())
        {
            if (path.endsWith(QLatin1Char('/')))
                child->updateIcon(iconProvider, path + child->fileName);
            else
                child->updateIcon(iconProvider, path + QLatin1Char('/') + child->fileName);
        }
        else
            child->updateIcon(iconProvider, child->fileName);
    }
}

void LSTrashNode::retranslateStrings(QFileIconProvider *iconProvider, const QString &path)
{
    for (LSTrashNode *child : qAsConst(children))
    {
        if (!path.isEmpty())
        {
            if (path.endsWith(QLatin1Char('/')))
                child->retranslateStrings(iconProvider, path + child->fileName);
            else
                child->retranslateStrings(iconProvider, path + QLatin1Char('/') + child->fileName);
        }
        else
            child->retranslateStrings(iconProvider, child->fileName);
    }
}
