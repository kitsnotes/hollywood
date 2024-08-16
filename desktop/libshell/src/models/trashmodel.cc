// Hollywood Shell Library
// (C) 2024 Originull Software
// Portions Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LGPL-2.1

#include "trashmodel_p.h"
#include "trashmodel.h"
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

#include <QHash>
#include <QStandardPaths>

LSTrashModel::LSTrashModel(QObject *parent) :
    LSTrashModel(*new LSTrashModelPrivate, parent)
{
    setRootPath();
}

void LSTrashModel::init()
{
    setRootPath();
}

LSTrashModel::LSTrashModel(LSTrashModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(dd, parent)
{
    Q_D(LSTrashModel);
    d->init();
}

LSTrashModel::~LSTrashModel() = default;

QModelIndex LSTrashModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const LSTrashModel);
    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
        return QModelIndex();

    // get the parent node
    LSTrashNode *parentNode = (d->indexValid(parent) ? d->node(parent) :
                                const_cast<LSTrashNode*>(&d->root));
    Q_ASSERT(parentNode);
    // now get the internal pointer for the index

    const int i = d->translateVisibleLocation(parentNode, row);
    if (i >= parentNode->visibleChildren.size())
        return QModelIndex();
    const QString &childName = parentNode->visibleChildren.at(i);
    const LSTrashNode *indexNode = parentNode->children.value(childName);
    Q_ASSERT(indexNode);

    return createIndex(row, column, const_cast<LSTrashNode*>(indexNode));
}

QModelIndex LSTrashModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (row == idx.row() && column < LSTrashModelPrivate::NumColumns)
        return createIndex(row, column, idx.internalPointer());
    else
        return QAbstractItemModel::sibling(row, column, idx);
}

QModelIndex LSTrashModel::index(const QString &path, int column) const
{
    Q_D(const LSTrashModel);
    LSTrashNode *node = d->node(path, false);
    return d->index(node, column);
}

LSTrashNode *LSTrashModelPrivate::node(const QModelIndex &index) const
{
    if (!index.isValid())
        return const_cast<LSTrashNode*>(&root);
    LSTrashNode *indexNode = static_cast<LSTrashNode*>(index.internalPointer());
    Q_ASSERT(indexNode);
    return indexNode;
}

LSTrashNode *LSTrashModelPrivate::node(const QString &path, bool fetch) const
{
    Q_Q(const LSTrashModel);
    Q_UNUSED(q);
    if (path.isEmpty() || path == myComputer() || path.startsWith(rootDir.path()))
        return const_cast<LSTrashNode*>(&root);

    // Construct the nodes up to the new root path if they need to be built
    QString absolutePath;
    QString longPath = path;
    if (longPath == rootDir.path())
        absolutePath = rootDir.absolutePath();
    else
        absolutePath = QDir(longPath).absolutePath();

    // ### TODO can we use bool QAbstractFileEngine::caseSensitive() const?
    QStringList pathElements = absolutePath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if ((pathElements.isEmpty()) && QDir::fromNativeSeparators(longPath) != rootDir.path())
        return const_cast<LSTrashNode*>(&root);

    QModelIndex index = QModelIndex(); // start with "My Computer"
    QString elementPath;
    QChar separator = QLatin1Char('/');
    QString trailingSeparator;

    // add the "/" item, since it is a valid path element on Unix
    if (absolutePath[0] == QLatin1Char('/'))
        pathElements.prepend(QLatin1String("/"));

    LSTrashNode *parent = node(index);

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

        LSTrashNode *node;
        if (!alreadyExisted) {
            // Someone might call ::index("file://cookie/monster/doesn't/like/veggies"),
            // a path that doesn't exists, I.E. don't blindly create directories.
            QFileInfo info(elementPath);
            if (!info.exists())
                return const_cast<LSTrashNode*>(&root);
            LSTrashModelPrivate *p = const_cast<LSTrashModelPrivate*>(this);
            node = p->addNode(parent, element,info);
            node->populate(info);
        } else
            node = parent->children.value(element);

        Q_ASSERT(node);
        if (!node->m_visible) {
            // It has been filtered out
            if (alreadyExisted && !fetch)
                return const_cast<LSTrashNode*>(&root);

            LSTrashModelPrivate *p = const_cast<LSTrashModelPrivate*>(this);
            p->addVisibleFiles(parent, QStringList(element));
            if (!p->bypassFilters.contains(node))
                p->bypassFilters[node] = 1;
            QString dir = q->filePath(this->index(parent));
            if (fetch) {
                Fetching f = { std::move(dir), std::move(element), node };
                p->toFetch.append(std::move(f));
                p->fetchingTimer.start(0, const_cast<LSTrashModel*>(q));
            }
        }
        parent = node;
    }

    return parent;
}

void LSTrashModel::timerEvent(QTimerEvent *event)
{
    Q_D(LSTrashModel);
    if (event->timerId() == d->fetchingTimer.timerId()) {
        d->fetchingTimer.stop();
        for (int i = 0; i < d->toFetch.count(); ++i) {
            //const LSTrashNode *node = d->toFetch.at(i).node;
                d->fileInfoGatherer.fetchExtendedInformation(d->toFetch.at(i).dir,
                                                             QStringList(d->toFetch.at(i).file));
        }
        d->toFetch.clear();
    }
}

bool LSTrashModel::isDir(const QModelIndex &index) const
{
    // This function is for public usage only because it could create a file info
    Q_D(const LSTrashModel);
    if (!index.isValid())
        return false;
    return d->node(index)->isDir();
}

qint64 LSTrashModel::size(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    if (!index.isValid())
        return 0;
    return d->node(index)->size();
}

QString LSTrashModel::type(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    if (!index.isValid())
        return QString();
    return d->node(index)->type();
}

QDateTime LSTrashModel::deleteTime(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    if (!index.isValid())
        return QDateTime();
    return d->node(index)->deleteTime();
}

QString LSTrashModel::originalPath(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    if (!index.isValid())
        return QString();
    return d->node(index)->type();
}

QModelIndex LSTrashModel::parent(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    if (!d->indexValid(index))
        return QModelIndex();

    LSTrashNode *indexNode = d->node(index);
    Q_ASSERT(indexNode != nullptr);
    LSTrashNode *parentNode = indexNode->m_parent;
    if (parentNode == nullptr || parentNode == &d->root)
        return QModelIndex();

    // get the parent's row
    LSTrashNode *grandParentNode = parentNode->m_parent;
    Q_ASSERT(grandParentNode->children.contains(parentNode->fileName));
    int visualRow = d->translateVisibleLocation(grandParentNode, grandParentNode->visibleLocation(grandParentNode->children.value(parentNode->fileName)->fileName));
    if (visualRow == -1)
        return QModelIndex();
    return createIndex(visualRow, 0, parentNode);
}

QModelIndex LSTrashModelPrivate::index(const LSTrashNode *node, int column) const
{
    Q_Q(const LSTrashModel);
    LSTrashNode *parentNode = (node ? node->m_parent : nullptr);
    if (node == &root || node == &root || !parentNode)
        return QModelIndex();

    // get the parent's row
    Q_ASSERT(node);
    if (!node->m_visible)
        return QModelIndex();

    int visualRow = translateVisibleLocation(parentNode, parentNode->visibleLocation(node->fileName));
    return q->createIndex(visualRow, column, const_cast<LSTrashNode*>(node));
}

bool LSTrashModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const LSTrashModel);
    if (parent.column() > 0)
        return false;

    if (!parent.isValid()) // drives
        return true;

    const LSTrashNode *indexNode = d->node(parent);
    Q_ASSERT(indexNode);
    return (indexNode->isDir());
}

bool LSTrashModel::canFetchMore(const QModelIndex &parent) const
{
    Q_D(const LSTrashModel);
    if (!d->setRootPath)
        return false;
    const LSTrashNode *indexNode = d->node(parent);
    return (!indexNode->m_popChildren);
}

void LSTrashModel::fetchMore(const QModelIndex &parent)
{
    Q_D(LSTrashModel);
    LSTrashNode *indexNode = d->node(parent);
    if (indexNode->m_popChildren)
        return;
    indexNode->m_popChildren = true;
    if(parent == QModelIndex())
        d->fileInfoGatherer.list(d->m_trashDirectory+"/files");
    else
        d->fileInfoGatherer.list(filePath(parent));
}

int LSTrashModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const LSTrashModel);
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return d->root.visibleChildren.count();

    const LSTrashNode *parentNode = d->node(parent);
    return parentNode->visibleChildren.count();
}

int LSTrashModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : LSTrashModelPrivate::NumColumns;
}

QVariant LSTrashModel::myComputer(int role) const
{
    Q_D(const LSTrashModel);
    switch (role) {
    case Qt::DisplayRole:
        return LSTrashModelPrivate::myComputer();
    case Qt::DecorationRole:
#if QT_VERSION >= 0x060000
        return d->fileInfoGatherer.iconProvider()->icon(QAbstractFileIconProvider::Computer);
#else
        return d->fileInfoGatherer.iconProvider()->icon(QFileIconProvider::Computer);
#endif
    }
    return QVariant();
}


QVariant LSTrashModel::data(const QModelIndex &index, int role) const
{
    Q_D(const LSTrashModel);
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
        case 3: return d->deleteTime(index);
        default:
            //qWarning("data: invalid display value column %d", index.column());
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
            return icon;
        }
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    }

    return QVariant();
}

QString LSTrashModelPrivate::size(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    const LSTrashNode *n = node(index);
    if (n->isDir())
        return QLatin1String("--");

    return size(n->size());
}

QString LSTrashModelPrivate::deleteTime(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    const LSTrashNode *n = node(index);

    return n->deleteTime().toString();
}

QString LSTrashModelPrivate::size(qint64 bytes)
{
    return QLocale::system().formattedDataSize(bytes);
}

QString LSTrashModelPrivate::type(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();
    return node(index)->type();
}

QString LSTrashModelPrivate::name(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    LSTrashNode *dirNode = node(index);
    if (fileInfoGatherer.resolveSymlinks() &&
        !resolvedSymLinks.isEmpty()) {
        QString fullPath = QDir::fromNativeSeparators(filePath(index));
        return resolvedSymLinks.value(fullPath, dirNode->fileName);
    }
    return dirNode->fileName;
}

QString LSTrashModelPrivate::displayName(const QModelIndex &index) const
{
    if (!index.isValid())
        return QString();

    return name(index);
}

QIcon LSTrashModelPrivate::icon(const QModelIndex &index) const
{
    if (!index.isValid())
        return QIcon();
    return node(index)->icon();
}

bool LSTrashModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    Q_UNUSED(idx)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return false;
}

QVariant LSTrashModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    case 3: returnValue = tr("Date Deleted");
        break;
    case 4: returnValue = tr("Original Path");
        break;
    default: return QVariant();
    }
    return returnValue;
}

Qt::ItemFlags LSTrashModel::flags(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return flags;

    LSTrashNode *indexNode = d->node(index);
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
    return flags;
}

void LSTrashModelPrivate::_hwtrash_performDelayedSort()
{
    Q_Q(LSTrashModel);
    q->sort(sortColumn, sortOrder);
}


/*
    \internal
    Helper functor used by sort()
*/
class LSTrashModelSorter
{
public:
    inline LSTrashModelSorter(int column) : sortColumn(column)
    {
        naturalCompare.setNumericMode(true);
        naturalCompare.setCaseSensitivity(Qt::CaseInsensitive);
    }

    bool compareNodes(const LSTrashNode *l,
                      const LSTrashNode *r) const
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
            if (l->deleteTime() == r->deleteTime())
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return l->deleteTime() < r->deleteTime();
        }
        case 4:
        {
            int compare = naturalCompare.compare(l->originalPath(), r->originalPath());
            if (compare == 0)
                return naturalCompare.compare(l->fileName, r->fileName) < 0;

            return compare < 0;
        }
        }
        Q_ASSERT(false);
        return false;
    }

    bool operator()(const LSTrashNode *l,
                    const LSTrashNode *r) const
    {
        return compareNodes(l, r);
    }


private:
    QCollator naturalCompare;
    int sortColumn;
};

void LSTrashModelPrivate::sortChildren(int column, const QModelIndex &parent)
{
    Q_Q(LSTrashModel);
    LSTrashNode *indexNode = node(parent);
    if (indexNode->children.count() == 0)
        return;

    QList<LSTrashNode *> values;

    for (auto iterator = indexNode->children.constBegin(), cend = indexNode->children.constEnd(); iterator != cend; ++iterator) {
        if (filtersAcceptsNode(iterator.value())) {
            values.append(iterator.value());
        } else {
            iterator.value()->m_visible = false;
        }
    }
    LSTrashModelSorter ms(column);
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
            LSTrashNode *indexNode = node(childIndex);
            //Only do a recursive sort on visible nodes
            if (indexNode->m_visible)
                sortChildren(column, childIndex);
        }
    }
}

void LSTrashModel::sort(int column, Qt::SortOrder order)
{
    Q_D(LSTrashModel);
    if (d->sortOrder == order && d->sortColumn == column && !d->forceSort)
        return;

    emit layoutAboutToBeChanged();
    QModelIndexList oldList = persistentIndexList();
    QList<QPair<LSTrashNode *, int>> oldNodes;
    const int nodeCount = oldList.count();
    oldNodes.reserve(nodeCount);
    for (int i = 0; i < nodeCount; ++i) {
        const QModelIndex &oldNode = oldList.at(i);
        QPair<LSTrashNode*, int> pair(d->node(oldNode), oldNode.column());
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
        const QPair<LSTrashNode*, int> &oldNode = oldNodes.at(i);
        newList.append(d->index(oldNode.first, oldNode.second));
    }
    changePersistentIndexList(oldList, newList);
    emit layoutChanged();
    emit sortingChanged();
}

int LSTrashModel::sortColumn() const
{
    Q_D(const LSTrashModel);
    return d->sortColumn;
}

Qt::SortOrder LSTrashModel::sortOrder() const
{
    Q_D(const LSTrashModel);
    return d->sortOrder;
}

QStringList LSTrashModel::mimeTypes() const
{
    return QStringList(QLatin1String("text/uri-list"));
}

QMimeData *LSTrashModel::mimeData(const QModelIndexList &indexes) const
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

bool LSTrashModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                             int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    /* if (!parent.isValid() || isReadOnly())
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

    return success;*/
    return false;
}

Qt::DropActions LSTrashModel::supportedDropActions() const
{
    return Qt::MoveAction;
}


QHash<int, QByteArray> LSTrashModel::roleNames() const
{
    auto ret = QAbstractItemModel::roleNames();
    ret.insert(LSTrashModel::FileIconRole,
               QByteArrayLiteral("fileIcon")); // == Qt::decoration
    ret.insert(LSTrashModel::FilePathRole, QByteArrayLiteral("filePath"));
    ret.insert(LSTrashModel::FileNameRole, QByteArrayLiteral("fileName"));
    return ret;
}

void LSTrashModel::setOption(Option option, bool on)
{
    LSTrashModel::Options previousOptions = options();
    setOptions(previousOptions.setFlag(option, on));
}

bool LSTrashModel::testOption(Option option) const
{
    return options().testFlag(option);
}

void LSTrashModel::setOptions(Options options)
{
    const Options changed = (options ^ LSTrashModel::options());

#if QT_CONFIG(filesystemwatcher)
    Q_D(LSTrashModel);
    if (changed.testFlag(DontWatchForChanges))
        d->fileInfoGatherer.setWatching(!options.testFlag(DontWatchForChanges));
#endif

    if (changed.testFlag(DontUseCustomDirectoryIcons)) {
        if (auto provider = iconProvider()) {
#if QT_VERSION >= 0x060000
            QAbstractFileIconProvider::Options providerOptions = provider->options();
            providerOptions.setFlag(QAbstractFileIconProvider::DontUseCustomDirectoryIcons,
                                    options.testFlag(LSTrashModel::DontUseCustomDirectoryIcons));
            provider->setOptions(providerOptions);
#else
            QFileIconProvider::Options providerOptions = provider->options();
            providerOptions.setFlag(QFileIconProvider::DontUseCustomDirectoryIcons,
                                    options.testFlag(LSTrashModel::DontUseCustomDirectoryIcons));
            provider->setOptions(providerOptions);
#endif
        } else {
            qWarning("Setting LSTrashModel::DontUseCustomDirectoryIcons has no effect when no provider is used");
        }
    }
}

LSTrashModel::Options LSTrashModel::options() const
{
    LSTrashModel::Options result;
    result.setFlag(DontResolveSymlinks, true);
    Q_D(const LSTrashModel);
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

QString LSTrashModel::filePath(const QModelIndex &index) const
{
    Q_D(const LSTrashModel);
    QString fullPath = d->filePath(index);
    LSTrashNode *dirNode = d->node(index);
    if (dirNode->isDir()) {
        QFileInfo fullPathInfo(dirNode->fileInfo());
        QString canonicalPath = fullPathInfo.canonicalFilePath();
        auto *canonicalNode = d->node(fullPathInfo.canonicalFilePath(), false);
        QFileInfo resolvedInfo = canonicalNode->fileInfo();
        if (resolvedInfo.exists())
            return resolvedInfo.filePath();
    }
    return fullPath;
}

QString LSTrashModelPrivate::filePath(const QModelIndex &index) const
{
    Q_Q(const LSTrashModel);
    Q_UNUSED(q);
    if (!index.isValid())
        return QString();
    Q_ASSERT(index.model() == q);

    QStringList path;
    QModelIndex idx = index;
    while (idx.isValid()) {
        LSTrashNode *dirNode = node(idx);
        if (dirNode)
            path.prepend(dirNode->fileName);
        idx = idx.parent();
    }
    QString fullPath = QDir::fromNativeSeparators(path.join(QDir::separator()));
    if ((fullPath.length() > 2) && fullPath[0] == QLatin1Char('/') && fullPath[1] == QLatin1Char('/'))
        fullPath = fullPath.mid(1);
    return fullPath;
}

QModelIndex LSTrashModel::setRootPath()
{
    Q_D(LSTrashModel);
    QString newPath = d->trashDirectory()+"/files/";
    QString longNewPath = newPath;
    //we remove .. and . from the given path if exist
    if (!newPath.isEmpty())
        longNewPath = QDir::cleanPath(longNewPath);

    d->setRootPath = true;

    /*
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

    bool showDrives = (longNewPath.isEmpty() || longNewPath == LSTrashModelPrivate::myComputer());
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
    }*/

    // We have a new valid root path
    d->rootDir = QDir(longNewPath);
    QModelIndex newRootIndex;
    newRootIndex = d->index(d->rootDir.path());
    fetchMore(newRootIndex);
    emit rootPathChanged(longNewPath);
    d->forceSort = true;
    d->delayedSort();
    return newRootIndex;
}

QString LSTrashModel::rootPath() const
{
    Q_D(const LSTrashModel);
    return d->trashDirectory()+"/files";
}

QString LSTrashModel::xdgTrashDir()
{
    auto xdg = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first()+"/.local/share/Trash";
    auto xdgenv = qgetenv("XDG_DATA_HOME");

    if(!xdgenv.isEmpty())
    {
        QDir dir(xdgenv);
        if(dir.exists())
            xdg = dir.absolutePath()+"/Trash";
    }
    return xdg;
}

HWFileIconProvider *LSTrashModel::iconProvider() const
{
    Q_D(const LSTrashModel);
    return d->fileInfoGatherer.iconProvider();
}

void LSTrashModel::setFilter(QDir::Filters filters)
{
    Q_D(LSTrashModel);
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

QDir::Filters LSTrashModel::filter() const
{
    Q_D(const LSTrashModel);
    return d->filters;
}

void LSTrashModel::setNameFilterDisables(bool enable)
{
    Q_D(LSTrashModel);
    if (d->nameFilterDisables == enable)
        return;
    d->nameFilterDisables = enable;
    d->forceSort = true;
    d->delayedSort();
}

bool LSTrashModel::nameFilterDisables() const
{
    Q_D(const LSTrashModel);
    return d->nameFilterDisables;
}

void LSTrashModel::setNameFilters(const QStringList &filters)
{
#if QT_CONFIG(regularexpression)
    Q_D(LSTrashModel);

    if (!d->bypassFilters.isEmpty()) {
        // update the bypass filter to only bypass the stuff that must be kept around
        d->bypassFilters.clear();
        // We guarantee that rootPath will stick around
        QPersistentModelIndex root(index(rootPath()));
        const QModelIndexList persistentList = persistentIndexList();
        for (const auto &persistentIndex : persistentList) {
            LSTrashNode *node = d->node(persistentIndex);
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

QStringList LSTrashModel::nameFilters() const
{
    Q_D(const LSTrashModel);
    return d->nameFilters;
}

bool LSTrashModel::event(QEvent *event)
{
    Q_D(LSTrashModel);
    if (event->type() == QEvent::LanguageChange) {
        d->root.retranslateStrings(d->fileInfoGatherer.iconProvider(), QString());
        return true;
    }
    return QAbstractItemModel::event(event);
}

void LSTrashModelPrivate::_hwtrash_directoryChanged(const QString &directory, const QStringList &files)
{
    LSTrashNode *parentNode = node(directory, false);
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

LSTrashNode* LSTrashModelPrivate::addNode(LSTrashNode *parentNode, const QString &fileName, const QFileInfo& info)
{
    // In the common case, itemLocation == count() so check there first
    LSTrashNode *node = new LSTrashNode(fileName, parentNode);
    node->m_visible = true;
    QFileInfo newInfo(info);
    node->populate(newInfo);
    Q_ASSERT(!parentNode->children.contains(fileName));
    parentNode->children.insert(fileName, node);
    parentNode->visibleChildren.append(fileName);

    return node;
}

void LSTrashModelPrivate::removeNode(LSTrashNode *parentNode, const QString& name)
{
    Q_Q(LSTrashModel);
    QModelIndex parent = index(parentNode);
    bool indexHidden = isHiddenByFilter(parentNode, parent);

    int vLocation = parentNode->visibleLocation(name);
    if (vLocation >= 0 && !indexHidden)
        q->beginRemoveRows(parent, translateVisibleLocation(parentNode, vLocation),
                           translateVisibleLocation(parentNode, vLocation));
    LSTrashNode * node = parentNode->children.take(name);
    delete node;
    // cleanup sort files after removing rather then re-sorting which is O(n)
    if (vLocation >= 0)
        parentNode->visibleChildren.removeAt(vLocation);
    if (vLocation >= 0 && !indexHidden)
        q->endRemoveRows();
}

void LSTrashModelPrivate::addVisibleFiles(LSTrashNode *parentNode, const QStringList &newFiles)
{
    Q_Q(LSTrashModel);
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

void LSTrashModelPrivate::removeVisibleFile(LSTrashNode *parentNode, int vLocation)
{
    Q_Q(LSTrashModel);
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

void LSTrashModelPrivate::_hwtrash_fileSystemChanged(const QString &path,
                                            const QVector<QPair<QString, QFileInfo>> &updates)
{
#if QT_CONFIG(filesystemwatcher)
    Q_Q(LSTrashModel);
    QList<QString> rowsToUpdate;
    QStringList newFiles;
    LSTrashNode *parentNode = node(path, false);
    QModelIndex parentIndex = index(parentNode);
    for (const auto &update : updates) {
        QString fileName = update.first;
        if(fileName == "." || fileName == "..")
            continue;
        Q_ASSERT(!fileName.isEmpty());
        QFileInfo info(update.second);
        bool previouslyHere = parentNode->children.contains(fileName);
        if (!previouslyHere) {
            addNode(parentNode, fileName, info);
        }
        LSTrashNode * node = parentNode->children.value(fileName);

        if (*node != info ) {
            node->populate(info);
            bypassFilters.remove(node);
            // brand new information.
            if (!node->m_visible) {
                newFiles.append(fileName);
            } else {
                rowsToUpdate.append(fileName);
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

    if (newFiles.count() > 0 /*|| (sortColumn != 0 && rowsToUpdate.count() > 0)*/) {
        forceSort = true;
        delayedSort();
    }
#else
    Q_UNUSED(path);
    Q_UNUSED(updates);
#endif // filesystemwatcher
}

void LSTrashModelPrivate::_hwtrash_resolvedName(const QString &fileName, const QString &resolvedName)
{
    resolvedSymLinks[fileName] = resolvedName;
}

void LSTrashModelPrivate::init()
{
    Q_Q(LSTrashModel);

    auto xdg = LSTrashModel::xdgTrashDir();
    m_trashDirectory = xdg;
    auto files = xdg+"/files";
    root.fileName = files;

    delayedSortTimer.setSingleShot(true);

    qRegisterMetaType<QList<QPair<QString, QFileInfo>>>();
    q->connect(&fileInfoGatherer, SIGNAL(newListOfFiles(QString,QStringList)),
               q, SLOT(_hwtrash_directoryChanged(QString,QStringList)));
    q->connect(&fileInfoGatherer, SIGNAL(updates(QString, QList<QPair<QString, QFileInfo>>)), q,
               SLOT(_hwtrash_fileSystemChanged(QString, QList<QPair<QString, QFileInfo>>)));
    q->connect(&fileInfoGatherer, SIGNAL(nameResolved(QString,QString)),
               q, SLOT(_hwtrash_resolvedName(QString,QString)));
    q->connect(&fileInfoGatherer, SIGNAL(directoryLoaded(QString)),
               q, SIGNAL(directoryLoaded(QString)));
    q->connect(&delayedSortTimer, SIGNAL(timeout()), q, SLOT(_hwtrash_performDelayedSort()), Qt::QueuedConnection);
}

bool LSTrashModelPrivate::filtersAcceptsNode(const LSTrashNode *node) const
{
    // always accept drives
    if (node->m_parent == &root || node->m_parent == &root || bypassFilters.contains(node))
        return true;

    const bool hideDot           = (filters & QDir::NoDot);
    const bool hideDotDot        = (filters & QDir::NoDotDot);

    // Note that we match the behavior of entryList and not QFileInfo on this.
    bool isDot    = (node->fileName == QLatin1String("."));
    bool isDotDot = (node->fileName == QLatin1String(".."));
    if ((hideDot && isDot) || (hideDotDot && isDotDot))
        return false;

    return nameFilterDisables || passNameFilters(node);
}

bool LSTrashModelPrivate::passNameFilters(const LSTrashNode *node) const
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
void LSTrashModelPrivate::rebuildNameFilterRegexps()
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

#include "moc_trashmodel.cpp"
