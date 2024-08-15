// Hollywood Shell Library
// (C) 2024 Originull Software
// Portions Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LGPL-2.1

#include "private/filesystemmodel_p.h"
#include "filesystemmodel.h"
#include <qlocale.h>
#include <qmimedata.h>
#include <qurl.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <QtCore/qcollator.h>
#include <algorithm>
#if QT_VERSION >= 0x060000
#include <QRegularExpression>
#endif
#include "fileinfo.h"

QFileInfo LSFSModel::fileInfo(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    return d->node(index)->fileInfo();
}

bool LSFSModel::remove(const QModelIndex &aindex)
{
    Q_D(LSFSModel);

    const QString path = d->filePath(aindex);
    const QFileInfo fileInfo(path);
    const bool success = (fileInfo.isFile() || fileInfo.isSymLink())
            ? QFile::remove(path) : QDir(path).removeRecursively();
    return success;
}

LSFSModel::LSFSModel(QObject *parent) :
    LSFSModel(*new LSFSModelPrivate, parent)
{
}

LSFSModel::LSFSModel(LSFSModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(dd, parent)
{
    Q_D(LSFSModel);
    d->init();
}

LSFSModel::~LSFSModel() = default;

QModelIndex LSFSModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const LSFSModel);
    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
        return QModelIndex();

    // get the parent node
    LSFSNode *parentNode = (d->indexValid(parent) ? d->node(parent) :
                                                   const_cast<LSFSNode*>(&d->root));
    Q_ASSERT(parentNode);

    // now get the internal pointer for the index
    const int i = d->translateVisibleLocation(parentNode, row);
    if (i >= parentNode->visibleChildren.size())
        return QModelIndex();
    const QString &childName = parentNode->visibleChildren.at(i);
    const LSFSNode *indexNode = parentNode->children.value(childName);
    Q_ASSERT(indexNode);

    return createIndex(row, column, const_cast<LSFSNode*>(indexNode));
}

QModelIndex LSFSModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (row == idx.row() && column < LSFSModelPrivate::NumColumns)
        return createIndex(row, column, idx.internalPointer());
    else
        return QAbstractItemModel::sibling(row, column, idx);
}

QModelIndex LSFSModel::index(const QString &path, int column) const
{
    Q_D(const LSFSModel);
    LSFSNode *node = d->node(path, false);
    return d->index(node, column);
}

LSFSNode *LSFSModelPrivate::node(const QModelIndex &index) const
{
    if (!index.isValid())
        return const_cast<LSFSNode*>(&root);
    LSFSNode *indexNode = static_cast<LSFSNode*>(index.internalPointer());
    Q_ASSERT(indexNode);
    return indexNode;
}

LSFSNode *LSFSModelPrivate::node(const QString &path, bool fetch) const
{
    Q_Q(const LSFSModel);
    Q_UNUSED(q);
    if (path.isEmpty() || path == myComputer() || path.startsWith(QLatin1Char(':')))
        return const_cast<LSFSNode*>(&root);

    // Construct the nodes up to the new root path if they need to be built
    QString absolutePath;
    QString longPath = path;
    if (longPath == rootDir.path())
        absolutePath = rootDir.absolutePath();
    else
        absolutePath = QDir(longPath).absolutePath();

    // ### TODO can we use bool QAbstractFileEngine::caseSensitive() const?
    QStringList pathElements = absolutePath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if ((pathElements.isEmpty()) && QDir::fromNativeSeparators(longPath) != QLatin1String("/"))
        return const_cast<LSFSNode*>(&root);

    QModelIndex index = QModelIndex(); // start with "My Computer"
    QString elementPath;
    QChar separator = QLatin1Char('/');
    QString trailingSeparator;

    // add the "/" item, since it is a valid path element on Unix
    if (absolutePath[0] == QLatin1Char('/'))
        pathElements.prepend(QLatin1String("/"));

    LSFSNode *parent = node(index);

    for (int i = 0; i < pathElements.count(); ++i) {
        QString element = pathElements.at(i);
        if (i != 0)
            elementPath.append(separator);
        elementPath.append(element);
        if (i == pathElements.count() - 1)
            elementPath.append(trailingSeparator);

        bool alreadyExisted = parent->children.contains(element);

        // we couldn't find the path element, we create a new node since we
        // _know_ that the path is valid
        if (alreadyExisted) {
            if ((parent->children.count() == 0)
                || (parent->caseSensitive()
                    && parent->children.value(element)->fileName != element)
                || (!parent->caseSensitive()
                    && parent->children.value(element)->fileName.toLower() != element.toLower()))
                alreadyExisted = false;
        }

        LSFSNode *node;
        if (!alreadyExisted) {
            // Someone might call ::index("file://cookie/monster/doesn't/like/veggies"),
            // a path that doesn't exists, I.E. don't blindly create directories.
            QFileInfo info(elementPath);
            if (!info.exists())
                return const_cast<LSFSNode*>(&root);
            LSFSModelPrivate *p = const_cast<LSFSModelPrivate*>(this);
            node = p->addNode(parent, element,info);
            node->populate(fileInfoGatherer.getInfo(info));
        } else
            node = parent->children.value(element);

        Q_ASSERT(node);
        if (!node->m_visible) {
            // It has been filtered out
            if (alreadyExisted && node->hasInformation() && !fetch)
                return const_cast<LSFSNode*>(&root);

            LSFSModelPrivate *p = const_cast<LSFSModelPrivate*>(this);
            p->addVisibleFiles(parent, QStringList(element));
            if (!p->bypassFilters.contains(node))
                p->bypassFilters[node] = 1;
            QString dir = q->filePath(this->index(parent));
            if (!node->hasInformation() && fetch) {
                Fetching f = { std::move(dir), std::move(element), node };
                p->toFetch.append(std::move(f));
                p->fetchingTimer.start(0, const_cast<LSFSModel*>(q));
            }
        }
        parent = node;
    }

    return parent;
}

void LSFSModel::timerEvent(QTimerEvent *event)
{
    Q_D(LSFSModel);
    if (event->timerId() == d->fetchingTimer.timerId()) {
        d->fetchingTimer.stop();
        for (int i = 0; i < d->toFetch.count(); ++i) {
            const LSFSNode *node = d->toFetch.at(i).node;
            if (!node->hasInformation()) {
                d->fileInfoGatherer.fetchExtendedInformation(d->toFetch.at(i).dir,
                                                 QStringList(d->toFetch.at(i).file));
            } else {
                // qDebug("yah!, you saved a little gerbil soul");
            }
        }
        d->toFetch.clear();
    }
}

bool LSFSModel::isDir(const QModelIndex &index) const
{
    // This function is for public usage only because it could create a file info
    Q_D(const LSFSModel);
    if (!index.isValid())
        return false;
    LSFSNode *n = d->node(index);
    if (n->hasInformation())
        return n->isDir();
    return fileInfo(index).isDir();
}

bool LSFSModel::isDesktop(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    if (!index.isValid())
        return false;
    LSFSNode *n = d->node(index);
    return n->isDesktopFile();
}

QString LSFSModel::desktopFileForIndex(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    if (!index.isValid())
        return QString();
    LSFSNode *n = d->node(index);
    if(!n->isDesktopFile())
        return QString();

    return n->fileInfo().absoluteFilePath();
}

qint64 LSFSModel::size(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    if (!index.isValid())
        return 0;
    return d->node(index)->size();
}

QString LSFSModel::type(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    if (!index.isValid())
        return QString();
    return d->node(index)->type();
}

QDateTime LSFSModel::lastModified(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    if (!index.isValid())
        return QDateTime();
    return d->node(index)->lastModified();
}

QModelIndex LSFSModel::parent(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    if (!d->indexValid(index))
        return QModelIndex();

    LSFSNode *indexNode = d->node(index);
    Q_ASSERT(indexNode != nullptr);
    LSFSNode *parentNode = indexNode->m_parent;
    if (parentNode == nullptr || parentNode == &d->root)
        return QModelIndex();

    // get the parent's row
    LSFSNode *grandParentNode = parentNode->m_parent;
    Q_ASSERT(grandParentNode->children.contains(parentNode->fileName));
    int visualRow = d->translateVisibleLocation(grandParentNode, grandParentNode->visibleLocation(grandParentNode->children.value(parentNode->fileName)->fileName));
    if (visualRow == -1)
        return QModelIndex();
    return createIndex(visualRow, 0, parentNode);
}

QModelIndex LSFSModelPrivate::index(const LSFSNode *node, int column) const
{
    Q_Q(const LSFSModel);
    LSFSNode *parentNode = (node ? node->m_parent : nullptr);
    if (node == &root || !parentNode)
        return QModelIndex();

    // get the parent's row
    Q_ASSERT(node);
    if (!node->m_visible)
        return QModelIndex();

    int visualRow = translateVisibleLocation(parentNode, parentNode->visibleLocation(node->fileName));
    return q->createIndex(visualRow, column, const_cast<LSFSNode*>(node));
}

bool LSFSModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const LSFSModel);
    if (parent.column() > 0)
        return false;

    if (!parent.isValid()) // drives
        return true;

    const LSFSNode *indexNode = d->node(parent);
    Q_ASSERT(indexNode);
    return (indexNode->isDir());
}

bool LSFSModel::canFetchMore(const QModelIndex &parent) const
{
    Q_D(const LSFSModel);
    if (!d->setRootPath)
        return false;
    const LSFSNode *indexNode = d->node(parent);
    return (!indexNode->m_popChildren);
}

void LSFSModel::fetchMore(const QModelIndex &parent)
{
    Q_D(LSFSModel);
    if (!d->setRootPath)
        return;
    LSFSNode *indexNode = d->node(parent);
    if (indexNode->m_popChildren)
        return;
    indexNode->m_popChildren = true;
    d->fileInfoGatherer.list(filePath(parent));
}

int LSFSModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const LSFSModel);
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return d->root.visibleChildren.count();

    const LSFSNode *parentNode = d->node(parent);
    return parentNode->visibleChildren.count();
}

int LSFSModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : LSFSModelPrivate::NumColumns;
}

QVariant LSFSModel::myComputer(int role) const
{
    Q_D(const LSFSModel);
    switch (role) {
    case Qt::DisplayRole:
        return LSFSModelPrivate::myComputer();
    case Qt::DecorationRole:
#if QT_VERSION >= 0x060000
        return d->fileInfoGatherer.iconProvider()->icon(QAbstractFileIconProvider::Computer);
#else
        return d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::Computer);
#endif
    }
    return QVariant();
}


QVariant LSFSModel::data(const QModelIndex &index, int role) const
{
    Q_D(const LSFSModel);
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
        if (index.column() == 0)
            return d->name(index);
        else
            return QVariant();
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0: return d->displayName(index);
        case 1: return d->size(index);
        case 2: return d->type(index);
        case 3: return d->time(index);
        case 4: return d->owner(index);
        case 5: return d->group(index);
        case 6: return d->comment(index);
        default:
            qWarning("data: invalid display value column %d", index.column());
            return QString();
        }
        break;
    case FilePathRole:
        return filePath(index);
    case FileNameRole:
        return d->name(index);
    case Qt::DecorationRole:
        if (index.column() == 0) {
            QIcon icon = d->icon(index);

            /*if (icon.isNull()) {
                if (d->node(index)->isDir())
                    icon = d->fileInfoGatherer.iconProvider()->icon(QAbstractFileIconProvider::Folder);
                else
                    icon = d->fileInfoGatherer.iconProvider()->icon(QAbstractFileIconProvider::File);
            }*/
            return icon;
        }
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    case FilePermissions:
        return (int)permissions(index);
    case FileSymlink:
        return (bool)isSymlink(index);
    case FileHidden:
        return (bool)isHidden(index);
#if QT_VERSION >= 0x060000
    // TODO: hack...
    case FileInfoRole:
        return QVariant::fromValue(d->node(index)->info);
#endif
    }

    return QVariant();
}

QString LSFSModelPrivate::size(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    const LSFSNode *n = node(index);
    if (n->isDir())
        return QLatin1String("--");

    return size(n->size());
}

QString LSFSModelPrivate::size(qint64 bytes)
{
    return QLocale::system().formattedDataSize(bytes);
}


QString LSFSModelPrivate::time(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    return QLocale::system().toString(node(index)->lastModified(), QLocale::ShortFormat);
}

QString LSFSModelPrivate::owner(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    return node(index)->owner();
}

QString LSFSModelPrivate::group(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    return node(index)->group();
}

QString LSFSModelPrivate::comment(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    return node(index)->xattrComment();
}

QString LSFSModelPrivate::type(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    return node(index)->type();
}

QString LSFSModelPrivate::name(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    LSFSNode *dirNode = node(index);
    if (fileInfoGatherer.resolveSymlinks() &&
        !resolvedSymLinks.isEmpty()) {
        QString fullPath = QDir::fromNativeSeparators(filePath(index));
        return resolvedSymLinks.value(fullPath, dirNode->fileName);
    }
    return dirNode->fileName;
}

QString LSFSModelPrivate::displayName(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    if(node(index)->isDesktopFile())
        return node(index)->desktopFileNameEntry();

    return name(index);
}

QIcon LSFSModelPrivate::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    return node(index)->icon();
}

bool LSFSModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    Q_D(LSFSModel);
    if (!idx.isValid()
        || idx.column() != 0
        || role != Qt::EditRole
        || (flags(idx) & Qt::ItemIsEditable) == 0) {
        return false;
    }

    QString newName = value.toString();
    QString oldName = idx.data().toString();
    if (newName == oldName)
        return true;

    const QString parentPath = filePath(parent(idx));

    if (newName.isEmpty() || QDir::toNativeSeparators(newName).contains(QDir::separator()))
        return false;


    if (!QDir(parentPath).rename(oldName, newName)) {
        return false;
    } else {
        /*
            *After re-naming something we don't want the selection to change*
            - can't remove rows and later insert
            - can't quickly remove and insert
            - index pointer can't change because treeview doesn't use persistent index's

            - if this get any more complicated think of changing it to just
              use layoutChanged
         */

        LSFSNode *indexNode = d->node(idx);
        LSFSNode *parentNode = indexNode->m_parent;
        int visibleLocation = parentNode->visibleLocation(parentNode->children.value(indexNode->fileName)->fileName);

        parentNode->visibleChildren.removeAt(visibleLocation);
        std::unique_ptr<LSFSNode> nodeToRename(parentNode->children.take(oldName));
        nodeToRename->fileName = newName;
        nodeToRename->m_parent = parentNode;
        nodeToRename->populate(d->fileInfoGatherer.getInfo(QFileInfo(parentPath, newName)));
        nodeToRename->m_visible = true;
        parentNode->children[newName] = nodeToRename.release();
        parentNode->visibleChildren.insert(visibleLocation, newName);

        d->delayedSort();
        emit fileRenamed(parentPath, oldName, newName);
    }
    return true;
}

QVariant LSFSModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        if (section == 0) {
            // ### TODO oh man this is ugly and doesn't even work all the way!
            // it is still 2 pixels off
            QImage pixmap(16, 1, QImage::Format_ARGB32_Premultiplied);
            pixmap.fill(Qt::transparent);
            return pixmap;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);

    QString returnValue;
    switch (section) {
    case 0: returnValue = tr("Name");
            break;
    case 1: returnValue = tr("Size");
            break;
    case 2: returnValue = tr("Kind");
           break;
    case 3: returnValue = tr("Date Modified");
            break;
    case 4: returnValue = tr("Owner");
            break;
    case 5: returnValue = tr("Group");
            break;
    case 6: returnValue = tr("Comment");
            break;
    default: return QVariant();
    }
    return returnValue;
}

Qt::ItemFlags LSFSModel::flags(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return flags;

    LSFSNode *indexNode = d->node(index);
    if (d->nameFilterDisables && !d->passNameFilters(indexNode)) {
        flags &= ~Qt::ItemIsEnabled;
        // ### TODO you shouldn't be able to set this as the current item, task 119433
        return flags;
    }

    flags |= Qt::ItemIsDragEnabled;

    if (!indexNode->isDir())
        flags |= Qt::ItemNeverHasChildren;
    if (d->readOnly)
        return flags;
    if ((index.column() == 0) && indexNode->permissions() & QFile::WriteUser) {
        flags |= Qt::ItemIsEditable;
        if (indexNode->isDir())
            flags |= Qt::ItemIsDropEnabled;
    }
    return flags;
}

void LSFSModelPrivate::_q_performDelayedSort()
{
    Q_Q(LSFSModel);
    q->sort(sortColumn, sortOrder);
}


/*
    \internal
    Helper functor used by sort()
*/
class LSFSModelSorter
{
public:
    inline LSFSModelSorter(int column) : sortColumn(column)
    {
        naturalCompare.setNumericMode(true);
        naturalCompare.setCaseSensitivity(Qt::CaseInsensitive);
    }

    bool compareNodes(const LSFSNode *l,
                    const LSFSNode *r) const
    {
        switch (sortColumn) {
        case 0: {
#ifndef Q_OS_MAC
            // place directories before files
            bool left = l->isDir();
            bool right = r->isDir();
            if (left ^ right)
                return left;
#endif
            return naturalCompare.compare(l->fileName, r->fileName) < 0;
                }
        case 1:
        {
            // Directories go first
            bool left = l->isDir();
            bool right = r->isDir();
            if (left ^ right)
                return left;

            qint64 sizeDifference = l->size() - r->size();
            if (sizeDifference == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return sizeDifference < 0;
        }
        case 2:
        {
            int compare = naturalCompare.compare(l->type(), r->type());
            if (compare == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return compare < 0;
        }
        case 3:
        {
            if (l->lastModified() == r->lastModified())
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return l->lastModified() < r->lastModified();
        }
        case 4:
        {
            int compare = naturalCompare.compare(l->owner(), r->owner());
            if (compare == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return compare < 0;
        }
        case 5:
        {
            int compare = naturalCompare.compare(l->group(), r->group());
            if (compare == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return compare < 0;
        }
        case 6:
        {
            int compare = naturalCompare.compare(l->xattrComment(), r->xattrComment());
            if (compare == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return compare < 0;
        }
        }
        Q_ASSERT(false);
        return false;
    }

    bool operator()(const LSFSNode *l,
                    const LSFSNode *r) const
    {
        return compareNodes(l, r);
    }


private:
    QCollator naturalCompare;
    int sortColumn;
};

void LSFSModelPrivate::sortChildren(int column, const QModelIndex &parent)
{
    Q_Q(LSFSModel);
    LSFSNode *indexNode = node(parent);
    if (indexNode->children.count() == 0)
        return;

    QList<LSFSNode *> values;

    for (auto iterator = indexNode->children.constBegin(), cend = indexNode->children.constEnd(); iterator != cend; ++iterator) {
        if (filtersAcceptsNode(iterator.value())) {
            values.append(iterator.value());
        } else {
            iterator.value()->m_visible = false;
        }
    }
    LSFSModelSorter ms(column);
    std::sort(values.begin(), values.end(), ms);
    // First update the new visible list
    indexNode->visibleChildren.clear();
    //No more dirty item we reset our internal dirty index
    indexNode->m_dirtyChildrenIndex = -1;
    const int numValues = values.count();
    indexNode->visibleChildren.reserve(numValues);
    for (int i = 0; i < numValues; ++i) {
        indexNode->visibleChildren.append(values.at(i)->fileName);
        values.at(i)->m_visible = true;
    }

    if (!disableRecursiveSort) {
        for (int i = 0; i < q->rowCount(parent); ++i) {
            const QModelIndex childIndex = q->index(i, 0, parent);
            LSFSNode *indexNode = node(childIndex);
            //Only do a recursive sort on visible nodes
            if (indexNode->m_visible)
                sortChildren(column, childIndex);
        }
    }
}

void LSFSModel::sort(int column, Qt::SortOrder order)
{
    Q_D(LSFSModel);
    if (d->sortOrder == order && d->sortColumn == column && !d->forceSort)
        return;

    emit layoutAboutToBeChanged();
    QModelIndexList oldList = persistentIndexList();
    QList<QPair<LSFSNode *, int>> oldNodes;
    const int nodeCount = oldList.count();
    oldNodes.reserve(nodeCount);
    for (int i = 0; i < nodeCount; ++i) {
        const QModelIndex &oldNode = oldList.at(i);
        QPair<LSFSNode*, int> pair(d->node(oldNode), oldNode.column());
        oldNodes.append(pair);
    }

    if (!(d->sortColumn == column && d->sortOrder != order && !d->forceSort)) {
        //we sort only from where we are, don't need to sort all the model
        d->sortChildren(column, index(rootPath()));
        d->sortColumn = column;
        d->forceSort = false;
    }
    d->sortOrder = order;

    QModelIndexList newList;
    const int numOldNodes = oldNodes.size();
    newList.reserve(numOldNodes);
    for (int i = 0; i < numOldNodes; ++i) {
        const QPair<LSFSNode*, int> &oldNode = oldNodes.at(i);
        newList.append(d->index(oldNode.first, oldNode.second));
    }
    changePersistentIndexList(oldList, newList);
    emit layoutChanged();
    emit sortingChanged();
}

int LSFSModel::sortColumn() const
{
    Q_D(const LSFSModel);
    return d->sortColumn;
}

Qt::SortOrder LSFSModel::sortOrder() const
{
    Q_D(const LSFSModel);
    return d->sortOrder;
}

QStringList LSFSModel::mimeTypes() const
{
    return QStringList(QLatin1String("text/uri-list"));
}

QMimeData *LSFSModel::mimeData(const QModelIndexList &indexes) const
{
    QList<QUrl> urls;
    QList<QModelIndex>::const_iterator it = indexes.begin();
    for (; it != indexes.end(); ++it)
        if ((*it).column() == 0)
            urls << QUrl::fromLocalFile(filePath(*it));
    QMimeData *data = new QMimeData();
    data->setUrls(urls);
    return data;
}

bool LSFSModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                             int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!parent.isValid() || isReadOnly())
        return false;

    bool success = true;
    QString to = filePath(parent) + QDir::separator();

    qDebug() << data->urls();

    QList<QUrl> urls = data->urls();
    QList<QUrl>::const_iterator it = urls.constBegin();

    switch (action) {
    case Qt::CopyAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::copy(path, to + QFileInfo(path).fileName()) && success;
        }
        break;
    case Qt::LinkAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::link(path, to + QFileInfo(path).fileName()) && success;
        }
        break;
    case Qt::MoveAction:
        for (; it != urls.constEnd(); ++it) {
            QString path = (*it).toLocalFile();
            success = QFile::rename(path, to + QFileInfo(path).fileName()) && success;
        }
        break;
    default:
        return false;
    }

    return success;
}

/*!
    \reimp
*/
Qt::DropActions LSFSModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}


QHash<int, QByteArray> LSFSModel::roleNames() const
{
    auto ret = QAbstractItemModel::roleNames();
    ret.insert(LSFSModel::FileIconRole,
               QByteArrayLiteral("fileIcon")); // == Qt::decoration
    ret.insert(LSFSModel::FilePathRole, QByteArrayLiteral("filePath"));
    ret.insert(LSFSModel::FileNameRole, QByteArrayLiteral("fileName"));
    ret.insert(LSFSModel::FilePermissions, QByteArrayLiteral("filePermissions"));
    return ret;
}

void LSFSModel::setOption(Option option, bool on)
{
    LSFSModel::Options previousOptions = options();
    setOptions(previousOptions.setFlag(option, on));
}

bool LSFSModel::testOption(Option option) const
{
    return options().testFlag(option);
}

void LSFSModel::setOptions(Options options)
{
    const Options changed = (options ^ LSFSModel::options());

    if (changed.testFlag(DontResolveSymlinks))
        setResolveSymlinks(!options.testFlag(DontResolveSymlinks));

#if QT_CONFIG(filesystemwatcher)
    Q_D(LSFSModel);
    if (changed.testFlag(DontWatchForChanges))
        d->fileInfoGatherer.setWatching(!options.testFlag(DontWatchForChanges));
#endif

    if (changed.testFlag(DontUseCustomDirectoryIcons)) {
        if (auto provider = iconProvider()) {
#if QT_VERSION >= 0x060000
            QAbstractFileIconProvider::Options providerOptions = provider->options();
            providerOptions.setFlag(QAbstractFileIconProvider::DontUseCustomDirectoryIcons,
                                    options.testFlag(LSFSModel::DontUseCustomDirectoryIcons));
            provider->setOptions(providerOptions);
#else
            QFileIconProvider::Options providerOptions = provider->options();
            providerOptions.setFlag(QFileIconProvider::DontUseCustomDirectoryIcons,
                                    options.testFlag(LSFSModel::DontUseCustomDirectoryIcons));
            provider->setOptions(providerOptions);
#endif
        } else {
            qWarning("Setting LSFSModel::DontUseCustomDirectoryIcons has no effect when no provider is used");
        }
    }
}

LSFSModel::Options LSFSModel::options() const
{
    LSFSModel::Options result;
    result.setFlag(DontResolveSymlinks, !resolveSymlinks());
    Q_D(const LSFSModel);
    result.setFlag(DontWatchForChanges, !d->fileInfoGatherer.isWatching());
    if (auto provider = iconProvider()) {
#if QT_VERSION >= 0x060000
        result.setFlag(DontUseCustomDirectoryIcons,
                       provider->options().testFlag(QAbstractFileIconProvider::DontUseCustomDirectoryIcons));
#else
        result.setFlag(DontUseCustomDirectoryIcons,
                       provider->options().testFlag(QFileIconProvider::DontUseCustomDirectoryIcons));
#endif
    }
    return result;
}

QString LSFSModel::filePath(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    QString fullPath = d->filePath(index);
    LSFSNode *dirNode = d->node(index);
    if (dirNode->isSymLink()
        && d->fileInfoGatherer.resolveSymlinks()
        && d->resolvedSymLinks.contains(fullPath)
        && dirNode->isDir()) {
        QFileInfo fullPathInfo(dirNode->fileInfo());
        if (!dirNode->hasInformation())
            fullPathInfo = QFileInfo(fullPath);
        QString canonicalPath = fullPathInfo.canonicalFilePath();
        auto *canonicalNode = d->node(fullPathInfo.canonicalFilePath(), false);
        QFileInfo resolvedInfo = canonicalNode->fileInfo();
        if (!canonicalNode->hasInformation())
            resolvedInfo = QFileInfo(canonicalPath);
        if (resolvedInfo.exists())
            return resolvedInfo.filePath();
    }
    return fullPath;
}

QString LSFSModelPrivate::filePath(const QModelIndex &index) const
{
    Q_Q(const LSFSModel);
    Q_UNUSED(q);
    if (!index.isValid())
        return QString();
    Q_ASSERT(index.model() == q);

    QStringList path;
    QModelIndex idx = index;
    while (idx.isValid()) {
        LSFSNode *dirNode = node(idx);
        if (dirNode)
            path.prepend(dirNode->fileName);
        idx = idx.parent();
    }
    QString fullPath = QDir::fromNativeSeparators(path.join(QDir::separator()));
    if ((fullPath.length() > 2) && fullPath[0] == QLatin1Char('/') && fullPath[1] == QLatin1Char('/'))
        fullPath = fullPath.mid(1);
    return fullPath;
}

QModelIndex LSFSModel::mkdir(const QModelIndex &parent, const QString &name)
{
    Q_D(LSFSModel);
    if (!parent.isValid())
        return parent;

    QDir dir(filePath(parent));
    if (!dir.mkdir(name))
        return QModelIndex();
    LSFSNode *parentNode = d->node(parent);
    d->addNode(parentNode, name, QFileInfo());
    Q_ASSERT(parentNode->children.contains(name));
    LSFSNode *node = parentNode->children[name];
    node->populate(d->fileInfoGatherer.getInfo(QFileInfo(dir.absolutePath() + QDir::separator() + name)));
    d->addVisibleFiles(parentNode, QStringList(name));
    return d->index(node);
}

QFile::Permissions LSFSModel::permissions(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    return d->node(index)->permissions();
}

bool LSFSModel::isHidden(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    return d->node(index)->isHidden();
}

bool LSFSModel::isSymlink(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    return d->node(index)->isSymLink();
}

QString LSFSModel::absolutePath(const QModelIndex &index) const
{
    Q_D(const LSFSModel);
    return d->node(index)->fileInfo().absoluteFilePath();
}

QModelIndex LSFSModel::setRootPath(const QString &newPath)
{
    Q_D(LSFSModel);
    QString longNewPath = newPath;
    //we remove .. and . from the given path if exist
    if (!newPath.isEmpty())
        longNewPath = QDir::cleanPath(longNewPath);

    d->setRootPath = true;

    //user don't ask for the root path ("") but the conversion failed
    if (!newPath.isEmpty() && longNewPath.isEmpty())
        return d->index(rootPath());

    if (d->rootDir.path() == longNewPath)
        return d->index(rootPath());

    auto node = d->node(longNewPath);
    QFileInfo newPathInfo;
    if (node && node->hasInformation())
        newPathInfo = node->fileInfo();
    else
        newPathInfo = QFileInfo(longNewPath);

    bool showDrives = (longNewPath.isEmpty() || longNewPath == LSFSModelPrivate::myComputer());
    if (!showDrives && !newPathInfo.exists())
        return d->index(rootPath());

    //We remove the watcher on the previous path
    if (!rootPath().isEmpty() && rootPath() != QLatin1String(".")) {
        //This remove the watcher for the old rootPath
       d->fileInfoGatherer.removePath(rootPath());
        //This line "marks" the node as dirty, so the next fetchMore
        //call on the path will ask the gatherer to install a watcher again
        //But it doesn't re-fetch everything
        d->node(rootPath())->m_popChildren = false;
    }

    // We have a new valid root path
    d->rootDir = QDir(longNewPath);
    QModelIndex newRootIndex;
    if (showDrives) {
        // otherwise dir will become '.'
        d->rootDir.setPath(QLatin1String(""));
    } else {
        newRootIndex = d->index(d->rootDir.path());
    }
    fetchMore(newRootIndex);
    emit rootPathChanged(longNewPath);
    d->forceSort = true;
    d->delayedSort();
    return newRootIndex;
}

QString LSFSModel::rootPath() const
{
    Q_D(const LSFSModel);
    return d->rootDir.path();
}

QDir LSFSModel::rootDirectory() const
{
    Q_D(const LSFSModel);
    QDir dir(d->rootDir);
    dir.setNameFilters(nameFilters());
    dir.setFilter(filter());
    return dir;
}

HWFileIconProvider *LSFSModel::iconProvider() const
{
    Q_D(const LSFSModel);
    return d->fileInfoGatherer.iconProvider();
}

void LSFSModel::setFilter(QDir::Filters filters)
{
    Q_D(LSFSModel);
    if (d->filters == filters)
        return;
    d->filters = filters;
#if QT_VERSION >= 0x060000
    const bool changingCaseSensitivity =
        filters.testFlag(QDir::CaseSensitive) != d->filters.testFlag(QDir::CaseSensitive);
    if (changingCaseSensitivity)
        d->rebuildNameFilterRegexps();
#else
    setNameFilters(nameFilters());
#endif
    d->forceSort = true;
    d->delayedSort();
}

QDir::Filters LSFSModel::filter() const
{
    Q_D(const LSFSModel);
    return d->filters;
}

void LSFSModel::setResolveSymlinks(bool enable)
{
    Q_D(LSFSModel);
    d->fileInfoGatherer.setResolveSymlinks(enable);
}

bool LSFSModel::resolveSymlinks() const
{
    Q_D(const LSFSModel);
    return d->fileInfoGatherer.resolveSymlinks();
}

void LSFSModel::setReadOnly(bool enable)
{
    Q_D(LSFSModel);
    d->readOnly = enable;
}

bool LSFSModel::isReadOnly() const
{
    Q_D(const LSFSModel);
    return d->readOnly;
}

void LSFSModel::setNameFilterDisables(bool enable)
{
    Q_D(LSFSModel);
    if (d->nameFilterDisables == enable)
        return;
    d->nameFilterDisables = enable;
    d->forceSort = true;
    d->delayedSort();
}

bool LSFSModel::nameFilterDisables() const
{
    Q_D(const LSFSModel);
    return d->nameFilterDisables;
}

void LSFSModel::setNameFilters(const QStringList &filters)
{
#if QT_CONFIG(regularexpression)
    Q_D(LSFSModel);

    if (!d->bypassFilters.isEmpty()) {
        // update the bypass filter to only bypass the stuff that must be kept around
        d->bypassFilters.clear();
        // We guarantee that rootPath will stick around
        QPersistentModelIndex root(index(rootPath()));
        const QModelIndexList persistentList = persistentIndexList();
        for (const auto &persistentIndex : persistentList) {
            LSFSNode *node = d->node(persistentIndex);
            while (node) {
                if (d->bypassFilters.contains(node))
                    break;
                if (node->isDir())
                    d->bypassFilters[node] = true;
                node = node->m_parent;
            }
        }
    }

    d->nameFilters = filters;
#if QT_VERSION >= 0x060000
    d->rebuildNameFilterRegexps();
#endif
    d->forceSort = true;
    d->delayedSort();
#else
    Q_UNUSED(filters);
#endif
}

QStringList LSFSModel::nameFilters() const
{
    Q_D(const LSFSModel);
    return d->nameFilters;
}

bool LSFSModel::event(QEvent *event)
{
    Q_D(LSFSModel);
    if (event->type() == QEvent::LanguageChange) {
        d->root.retranslateStrings(d->fileInfoGatherer.iconProvider(), QString());
        return true;
    }
    return QAbstractItemModel::event(event);
}

bool LSFSModel::rmdir(const QModelIndex &aindex)
{
    QString path = filePath(aindex);
    const bool success = QDir().rmdir(path);
    if (success) {
        LSFSModelPrivate * d = const_cast<LSFSModelPrivate*>(d_func());
        d->fileInfoGatherer.removePath(path);
    }
    return success;
}

void LSFSModelPrivate::_q_directoryChanged(const QString &directory, const QStringList &files)
{
    LSFSNode *parentNode = node(directory, false);
    if (parentNode->children.count() == 0)
        return;
    QStringList toRemove;
    QStringList newFiles = files;
    std::sort(newFiles.begin(), newFiles.end());
    for (auto i = parentNode->children.constBegin(), cend = parentNode->children.constEnd(); i != cend; ++i) {
        QStringList::iterator iterator = std::lower_bound(newFiles.begin(), newFiles.end(), i.value()->fileName);
        if ((iterator == newFiles.end()) || (i.value()->fileName < *iterator))
            toRemove.append(i.value()->fileName);
    }
    for (int i = 0 ; i < toRemove.count() ; ++i )
        removeNode(parentNode, toRemove[i]);
}

LSFSNode* LSFSModelPrivate::addNode(LSFSNode *parentNode, const QString &fileName, const QFileInfo& info)
{
    // In the common case, itemLocation == count() so check there first
    LSFSNode *node = new LSFSNode(fileName, parentNode);
    node->populate(info);
    Q_ASSERT(!parentNode->children.contains(fileName));
    parentNode->children.insert(fileName, node);
    return node;
}

void LSFSModelPrivate::removeNode(LSFSNode *parentNode, const QString& name)
{
    Q_Q(LSFSModel);
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);

    int vLocation = parentNode->visibleLocation(name);
    if (vLocation >= 0 && !indexHidden)
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                                       translateVisibleLocation(parentNode, vLocation));
    LSFSNode * node = parentNode->children.take(name);
    delete node;
    // cleanup sort files after removing rather then re-sorting which is O(n)
    if (vLocation >= 0)
        parentNode->visibleChildren.removeAt(vLocation);
    if (vLocation >= 0 && !indexHidden)
        q->endRemoveRows();
}

void LSFSModelPrivate::addVisibleFiles(LSFSNode *parentNode, const QStringList &newFiles)
{
    Q_Q(LSFSModel);
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);
    if (!indexHidden) {
        q->beginInsertRows(parent, parentNode->visibleChildren.count() , parentNode->visibleChildren.count() + newFiles.count() - 1);
    }

    if (parentNode->m_dirtyChildrenIndex == -1)
        parentNode->m_dirtyChildrenIndex = parentNode->visibleChildren.count();

    for (const auto &newFile : newFiles) {
        parentNode->visibleChildren.append(newFile);
        parentNode->children.value(newFile)->m_visible = true;
    }
    if (!indexHidden)
      q->endInsertRows();
}

void LSFSModelPrivate::removeVisibleFile(LSFSNode *parentNode, int vLocation)
{
    Q_Q(LSFSModel);
    if (vLocation == -1)
        return;
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);
    if (!indexHidden)
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                                       translateVisibleLocation(parentNode, vLocation));
    parentNode->children.value(parentNode->visibleChildren.at(vLocation))->m_visible = false;
    parentNode->visibleChildren.removeAt(vLocation);
    if (!indexHidden)
        q->endRemoveRows();
}

void LSFSModelPrivate::_q_fileSystemChanged(const QString &path,
                                                   const QVector<QPair<QString, QFileInfo>> &updates)
{
#if QT_CONFIG(filesystemwatcher)
    Q_Q(LSFSModel);
    QList<QString> rowsToUpdate;
    QStringList newFiles;
    LSFSNode *parentNode = node(path, false);
    QModelIndex parentIndex = index(parentNode);
    for (const auto &update : updates) {
        QString fileName = update.first;
        Q_ASSERT(!fileName.isEmpty());
        LSExtendedFileInfo info = fileInfoGatherer.getInfo(update.second);
        bool previouslyHere = parentNode->children.contains(fileName);
        if (!previouslyHere) {
            addNode(parentNode, fileName, info.fileInfo());
        }
        LSFSNode * node = parentNode->children.value(fileName);
        bool isCaseSensitive = parentNode->caseSensitive();
        if (isCaseSensitive) {
            if (node->fileName != fileName)
                continue;
        } else {
            if (QString::compare(node->fileName,fileName,Qt::CaseInsensitive) != 0)
                continue;
        }
        if (isCaseSensitive) {
            Q_ASSERT(node->fileName == fileName);
        } else {
            node->fileName = fileName;
        }

        if (*node != info ) {
            node->populate(info);
            bypassFilters.remove(node);
            // brand new information.
            if (filtersAcceptsNode(node)) {
                if (!node->m_visible) {
                    newFiles.append(fileName);
                } else {
                    rowsToUpdate.append(fileName);
                }
            } else {
                if (node->m_visible) {
                    int visibleLocation = parentNode->visibleLocation(fileName);
                    removeVisibleFile(parentNode, visibleLocation);
                } else {
                    // The file is not visible, don't do anything
                }
            }
        }
    }

    // bundle up all of the changed signals into as few as possible.
    std::sort(rowsToUpdate.begin(), rowsToUpdate.end());
    QString min;
    QString max;
    for (const QString &value : qAsConst(rowsToUpdate)) {
        //##TODO is there a way to bundle signals with QString as the content of the list?
        /*if (min.isEmpty()) {
            min = value;
            if (i != rowsToUpdate.count() - 1)
                continue;
        }
        if (i != rowsToUpdate.count() - 1) {
            if ((value == min + 1 && max.isEmpty()) || value == max + 1) {
                max = value;
                continue;
            }
        }*/
        max = value;
        min = value;
        int visibleMin = parentNode->visibleLocation(min);
        int visibleMax = parentNode->visibleLocation(max);
        if (visibleMin >= 0
            && visibleMin < parentNode->visibleChildren.count()
            && parentNode->visibleChildren.at(visibleMin) == min
            && visibleMax >= 0) {
            QModelIndex bottom = q->index(translateVisibleLocation(parentNode, visibleMin), 0, parentIndex);
            QModelIndex top = q->index(translateVisibleLocation(parentNode, visibleMax), 3, parentIndex);
            emit q->dataChanged(bottom, top);
        }

        /*min = QString();
        max = QString();*/
    }

    if (newFiles.count() > 0) {
        addVisibleFiles(parentNode, newFiles);
    }

    if (newFiles.count() > 0 || (sortColumn != 0 && rowsToUpdate.count() > 0)) {
        forceSort = true;
        delayedSort();
    }
#else
    Q_UNUSED(path);
    Q_UNUSED(updates);
#endif // filesystemwatcher
}

void LSFSModelPrivate::_q_resolvedName(const QString &fileName, const QString &resolvedName)
{
    resolvedSymLinks[fileName] = resolvedName;
}

void LSFSModelPrivate::init()
{
    Q_Q(LSFSModel);

    delayedSortTimer.setSingleShot(true);

    qRegisterMetaType<QList<QPair<QString, QFileInfo>>>();
    q->connect(&fileInfoGatherer, SIGNAL(newListOfFiles(QString,QStringList)),
               q, SLOT(_q_directoryChanged(QString,QStringList)));
    q->connect(&fileInfoGatherer, SIGNAL(updates(QString, QList<QPair<QString, QFileInfo>>)), q,
               SLOT(_q_fileSystemChanged(QString, QList<QPair<QString, QFileInfo>>)));
    q->connect(&fileInfoGatherer, SIGNAL(nameResolved(QString,QString)),
            q, SLOT(_q_resolvedName(QString,QString)));
    q->connect(&fileInfoGatherer, SIGNAL(directoryLoaded(QString)),
               q, SIGNAL(directoryLoaded(QString)));
    q->connect(&delayedSortTimer, SIGNAL(timeout()), q, SLOT(_q_performDelayedSort()), Qt::QueuedConnection);
}

bool LSFSModelPrivate::filtersAcceptsNode(const LSFSNode *node) const
{
    // always accept drives
    if (node->m_parent == &root || bypassFilters.contains(node))
        return true;

    // If we don't know anything yet don't accept it
    if (!node->hasInformation())
        return false;

    const bool filterPermissions = ((filters & QDir::PermissionMask)
                                   && (filters & QDir::PermissionMask) != QDir::PermissionMask);
    const bool hideDirs          = !(filters & (QDir::Dirs | QDir::AllDirs));
    const bool hideFiles         = !(filters & QDir::Files);
    const bool hideReadable      = !(!filterPermissions || (filters & QDir::Readable));
    const bool hideWritable      = !(!filterPermissions || (filters & QDir::Writable));
    const bool hideExecutable    = !(!filterPermissions || (filters & QDir::Executable));
    const bool hideHidden        = !(filters & QDir::Hidden);
    const bool hideSystem        = !(filters & QDir::System);
    const bool hideSymlinks      = (filters & QDir::NoSymLinks);
    const bool hideDot           = (filters & QDir::NoDot);
    const bool hideDotDot        = (filters & QDir::NoDotDot);

    // Note that we match the behavior of entryList and not QFileInfo on this.
    bool isDot    = (node->fileName == QLatin1String("."));
    bool isDotDot = (node->fileName == QLatin1String(".."));
    if (   (hideHidden && !(isDot || isDotDot) && node->isHidden())
        || (hideSystem && node->isSystem())
        || (hideDirs && node->isDir())
        || (hideFiles && node->isFile())
        || (hideSymlinks && node->isSymLink())
        || (hideReadable && node->isReadable())
        || (hideWritable && node->isWritable())
        || (hideExecutable && node->isExecutable())
        || (hideDot && isDot)
        || (hideDotDot && isDotDot))
        return false;

    return nameFilterDisables || passNameFilters(node);
}

bool LSFSModelPrivate::passNameFilters(const LSFSNode *node) const
{
#if QT_VERSION >= 0x060000
    if (nameFilters.isEmpty())
        return true;

    // Check the name regularexpression filters
    if (!(node->isDir() && (filters & QDir::AllDirs))) {
        const auto matchesNodeFileName = [node](const QRegularExpression &re)
        {
            return node->fileName.contains(re);
        };
        return std::any_of(nameFiltersRegexps.begin(),
                           nameFiltersRegexps.end(),
                           matchesNodeFileName);
    }
#else
    Q_UNUSED(node);
#endif
    return true;
}

#if QT_VERSION >= 0x060000
void LSFSModelPrivate::rebuildNameFilterRegexps()
{
    nameFiltersRegexps.clear();
    nameFiltersRegexps.reserve(nameFilters.size());
    const auto cs = (filters & QDir::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;
    const auto convertWildcardToRegexp = [cs](const QString &nameFilter)
    {
        return QRegularExpression::fromWildcard(nameFilter, cs);
    };
    std::transform(nameFilters.constBegin(),
                   nameFilters.constEnd(),
                   std::back_inserter(nameFiltersRegexps),
                   convertWildcardToRegexp);
}
#endif

#include "moc_filesystemmodel.cpp"
