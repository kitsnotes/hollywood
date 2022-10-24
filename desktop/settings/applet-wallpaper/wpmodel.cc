#include "wpmodel.h"
#include "chooser.h"

#include <QDir>
#include <QFile>
#include <QPair>
#include <QMimeDatabase>
#include <QMimeType>
#include <QImageReader>

WallpaperModel::WallpaperModel(QString target, QObject *parent)
    : QAbstractListModel(parent)
{
    // TODO: use a qfilesystemwatcher on the selected folder?
    // if our target is blank we will build a list of colors
    if(target.isEmpty())
    {
        m_colors = true;
        QFile file(":/Colors");
        if(file.open(QFile::ReadOnly))
        {
            auto data = file.readAll().split('\n');
            for(auto l : data)
            {
                auto vals = l.split(' ');
                auto color = vals.takeLast().toUpper();
                auto name = vals.join(' ');
                if(!name.isEmpty() && !color.isEmpty())
                    m_colorList.append(QPair<QString,QString>(name,color));
            }
        }
        file.close();

    }
    // else we are in a directory
    else
    {
        m_colors = false;
        QMimeDatabase db;
        auto support = QImageReader::supportedMimeTypes();
        auto dir = QDir(target);
        for(auto f : dir.entryInfoList(QDir::Files, QDir::Name))
        {
            auto m = db.mimeTypeForFile(f.absoluteFilePath());
            if(support.contains(m.name()))
                m_validFiles.append(f.absoluteFilePath());
        }
        m_directory = target;
    }
}

int WallpaperModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    if(m_colors)
        return m_colorList.count();

    return m_validFiles.count();
}

QModelIndex WallpaperModel::indexForItem(QString item)
{
    if(m_colors)
    {
        // search for a color
        for(int i = 0; i < m_colorList.count(); i++)
        {
            if(m_colorList[i].second.toUpper() == item.toUpper())
                return createIndex(i, 0);
        }
    }
    else
    {
        // search for a file
        for(int i = 0; i < m_validFiles.count(); i++)
        {
            if(m_validFiles[i] == item)
                return createIndex(i, 0);
        }
    }
    return QModelIndex();
}

bool WallpaperModel::hasItem(QString item)
{
    auto idx = indexForItem(item);
    if(idx.isValid())
        return true;
    else
        return false;
}

QString WallpaperModel::selectedDirectory() const
{
    return m_directory;
}

QVariant WallpaperModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        if(m_colors)
            return m_colorList.at(index.row()).first;
        else
        {
            QFileInfo f(m_validFiles.at(index.row()));
            auto strs = f.fileName().split('.');
            strs.removeLast();
            return strs.join('.');
        }
        break;
    case Qt::DecorationRole:
        if(m_colors)
            return createColorIcon(QColor(m_colorList.at(index.row()).second));
        else
        {
            QMimeDatabase db;
            auto m  = db.mimeTypeForFile(m_validFiles.at(index.row()));
            auto icon = QIcon::fromTheme(m.iconName(),
                                         QIcon::fromTheme(m.genericIconName()));

            return icon;
        }
        break;
    case Qt::UserRole+1:
        if(m_colors)
            return m_colorList.at(index.row()).second;
        else
            return m_validFiles.at(index.row());
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    }

    return QVariant();
}
