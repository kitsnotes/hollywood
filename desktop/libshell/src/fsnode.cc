#include "fsnode.h"
#include "fileinfo.h"
#include "desktopentry.h"

#include <QDateTime>

LSFSNode::LSFSNode(const QString &filename, LSFSNode *p)
    : fileName(filename), m_parent(p) {}

LSFSNode::~LSFSNode() {
    qDeleteAll(children);
    info.clear();
}

qint64 LSFSNode::size() const { if (info && !info.data()->isDir()) return info.data()->size(); return 0; }

QString LSFSNode::type() const
{
    if(m_desktop)
    {
        switch(m_desktop->type())
        {
        case LSDesktopEntry::TYPE_APP:
            return QApplication::tr("Application");
        case LSDesktopEntry::TYPE_DIR:
            return QApplication::tr("Directory");
        case LSDesktopEntry::TYPE_LINK:
            return QApplication::tr("URL");
        case LSDesktopEntry::TYPE_UNKNOWN:
            return QApplication::tr("Unknown .desktop entry");
        }
    }

    if (info)
        return info.data()->displayType;

    return QLatin1String("");
}

QDateTime LSFSNode::lastModified() const { if (info) return info.data()->lastModified(); return QDateTime(); }

QString LSFSNode::owner() const
{
    return info.data()->owner();
}

QString LSFSNode::group() const
{
    return info.data()->group();
}

QString LSFSNode::desktopFileNameEntry() const
{
    if(m_desktop)
    {
        QString localValue =
                m_desktop->localizedValue("Name").toString();

        return localValue.isEmpty() ? m_desktop->value("Name").toString() : localValue;
    }

    return QString();
}

QString LSFSNode::xattrComment() const
{
    char value[10000];
    getxattr(fileInfo().filePath().toUtf8().data(), "user.xdg.comment\0", value, 10000);
    QString xattrComment(value);
    return xattrComment;
}

QFileDevice::Permissions LSFSNode::permissions() const
{
    if (info)
        return info.data()->permissions();

    return (QFileDevice::Permissions)0;
}

bool LSFSNode::isDir() const
{
    if (info)
        return info.data()->isDir();
    if (children.count() > 0)
        return true;
    return false;
}

QFileInfo LSFSNode::fileInfo() const { if (info) return info.data()->fileInfo(); return QFileInfo(); }

bool LSFSNode::isFile() const { if (info) return info.data()->isFile(); return true; }

bool LSFSNode::isSystem() const { if (info) return info.data()->isSystem(); return true; }

bool LSFSNode::isHidden() const { if (info) return info.data()->isHidden(); return false; }

bool LSFSNode::isSymLink() const { if(info) return info.data()->isSymLink(); return false; }

/* TODO: fix this */
bool LSFSNode::caseSensitive() const { /* if (info) return info.data()->isCaseSensitive(); */ return true; }

bool LSFSNode::isDesktopFile() const
{
    return m_desktop ? true : false;
}

QIcon LSFSNode::icon() const
{
    if(m_desktop)
    {
        QString di =
                m_desktop->value("Icon").toString();
        if(!di.isEmpty())
        {
            QIcon dico = QIcon::fromTheme(di);
            if(!dico.isNull())
                return dico;
        }
    }
    if (info)
        return info.data()->icon;

    return QIcon();
}

bool LSFSNode::operator >(const QString &name) const
{
    if (caseSensitive())
        return fileName > name;
    return QString::compare(fileName, name, Qt::CaseInsensitive) > 0;
}

bool LSFSNode::operator <(const QString &name) const {
    if (caseSensitive())
        return fileName < name;
    return QString::compare(fileName, name, Qt::CaseInsensitive) < 0;
}

bool LSFSNode::operator !=(const LSExtendedFileInfo &fileInfo) const {
    return !operator==(fileInfo);
}

bool LSFSNode::operator ==(const QString &name) const
{
    if (caseSensitive())
        return fileName == name;
    return QString::compare(fileName, name, Qt::CaseInsensitive) == 0;
}

void LSFSNode::populate(const LSExtendedFileInfo &fileInfo)
{
    if (info.isNull())
        info = QSharedPointer<LSExtendedFileInfo>(new LSExtendedFileInfo(fileInfo.fileInfo()));

    if(info.data()->fileInfo().fileName().toLower().endsWith(".desktop"))
    {
        /* Check if we have a .desktop entry */
        auto *desktop = new LSDesktopEntry();
        if(desktop->load(info.data()->fileInfo().absoluteFilePath()))
            m_desktop = desktop;
        else
        {
            delete desktop;
            m_desktop = nullptr;
        }
    }
}

int LSFSNode::visibleLocation(const QString &childName)
{
    return visibleChildren.indexOf(childName);
}

bool LSFSNode::operator ==(const LSExtendedFileInfo &fileInfo) const
{
    return info && (*info == fileInfo);
}

bool LSFSNode::operator <(const LSFSNode &node) const
{
    if (caseSensitive() || node.caseSensitive())
        return fileName < node.fileName;
    return QString::compare(fileName, node.fileName, Qt::CaseInsensitive) < 0;
}

void LSFSNode::updateIcon(QFileIconProvider *iconProvider, const QString &path)
{
    if (!info.isNull())
        info.data()->icon = iconProvider->icon(QFileInfo(path));

    for (LSFSNode *child : qAsConst(children))
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

void LSFSNode::retranslateStrings(QFileIconProvider *iconProvider, const QString &path)
{
    if (!info.isNull())
        info.data()->displayType = iconProvider->type(QFileInfo(path));

    for (LSFSNode *child : qAsConst(children))
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
