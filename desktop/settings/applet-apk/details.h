#ifndef DETAILSDIALOG_H
#define DETAILSDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QTreeView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QAbstractListModel>
#include <QVBoxLayout>
#include <QtApkChangeset.h>
#include <QtApkTransaction.h>
#include <QtApkDatabaseAsync.h>

class DetailsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DetailsModel(QObject *parent = nullptr);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //bool insertRows(int row, int count, const QModelIndex &parent);
private:
    QtApk::DatabaseAsync *m_apkdb;
    QtApk::Transaction *m_trans____rightsareimportant;
};

class DetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DetailsDialog(QWidget *parent = nullptr);
private:
    QVBoxLayout *m_layout;
    QTreeView *m_view;
    QDialogButtonBox *m_buttons;
    DetailsModel *m_model;
    QtApk::Changeset *m_changeset;
};

#endif // DETAILSDIALOG_H
