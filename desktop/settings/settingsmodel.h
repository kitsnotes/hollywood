#ifndef SETTINGSAPPLETMODEL_H
#define SETTINGSAPPLETMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QDir>
#include <QIcon>

#include <hollywood/appletinterface.h>

class SettingsAppletModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    explicit SettingsAppletModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString description(const QModelIndex &index) const;
    QString categoryName(const QModelIndex &index) const;
    SettingsAppletInterface* appletForId(const QString &id) const;
    SettingsAppletInterface::Category category(const QModelIndex &index) const;
    SettingsAppletInterface* applet(const QModelIndex &index) const;
private:
    void scanPlugins();
    QPluginLoader *m_loader;
    QList<SettingsAppletInterface*> m_applets;
};

#endif // SETTINGSAPPLETMODEL_H
