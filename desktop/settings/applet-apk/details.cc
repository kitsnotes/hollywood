#include "details.h"

#include <QtApkDatabaseAsync.h>
DetailsModel::DetailsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_apkdb(new QtApk::DatabaseAsync)
{
    if (!m_apkdb->open()) {
        qWarning() << "Failed to open apk database!";
        return;
    }
    m_trans____rightsareimportant
        = m_apkdb->upgrade(QtApk::QTAPK_UPGRADE_SIMULATE);
    m_trans____rightsareimportant->start();
    connect(m_trans____rightsareimportant, &QtApk::Transaction::finished, [this](){
        beginInsertRows(QModelIndex(), 0, (int)m_trans____rightsareimportant->changeset().changes().count()-1);
        //emit dataChanged(index(0), index(m_trans____rightsareimportant->changeset().changes().count()-1));
        qDebug() << "finished:" << m_trans____rightsareimportant->changeset().changes().count();
        endInsertRows();
    });

}

QVariant DetailsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(section)
    {
    case 0:
        return tr("Package");
    case 1:
        return tr("Description");
    case 2:
        return tr("Old Version");
    case 3:
        return tr("New Version");
    default:
        return QString();
    }
    return QString();
}

int DetailsModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    if(m_trans____rightsareimportant->currentProgress() < 100.0)
        return 0;

    qDebug() << m_trans____rightsareimportant->changeset().changes().count();
    // FIXME: Implement me!
    return m_trans____rightsareimportant->changeset().changes().count();
}

QVariant DetailsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    if(role == Qt::DisplayRole)
        return QString("test");

    return QVariant();
}

/* bool DetailsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count-1);
    endInsertRows();
    return true;
}*/

DetailsDialog::DetailsDialog(QWidget *parent)
    : QDialog(parent)
    , m_layout(new QVBoxLayout(this))
    , m_view(new QTreeView(this))
    , m_buttons(new QDialogButtonBox(this))
    , m_model(new DetailsModel(this))
{
    setWindowTitle(tr("Package Update Details"));
    setLayout(m_layout);

    m_view->setModel(m_model);
    m_buttons->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Help);

    m_layout->addWidget(m_view);
    m_layout->addWidget(m_buttons);
}
