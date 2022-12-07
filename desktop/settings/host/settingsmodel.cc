#include "settingsmodel.h"

#define HOLLYWOOD_SETTINGS_PLUGIN_DIR "/usr/libexec/hollywood/settings"

SettingsAppletModel::SettingsAppletModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    scanPlugins();
}

int SettingsAppletModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_applets.count();
}

int SettingsAppletModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant SettingsAppletModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
            case 0: return name(index);
            case 1: return description(index);
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return icon(index);
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    case 0x17CE990A:
        if(index.column() == 0)
            return QVariant(categoryName(index));
        break;
    case 0x27857E60:
        if(index.column() == 0)
            return QVariant(category(index));
    }

    return QVariant();
}

QIcon SettingsAppletModel::icon(const QModelIndex &index) const
{
    return m_applets.at(index.row())->icon();
}

QString SettingsAppletModel::name(const QModelIndex &index) const
{
    return m_applets.at(index.row())->name();
}

QString SettingsAppletModel::description(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QString("");
}

QString SettingsAppletModel::categoryName(const QModelIndex &index) const
{
    auto category = m_applets.at(index.row())->category();
    switch(category)
    {
    case SettingsAppletInterface::Personal:
        return QLatin1String("Personal Settings");
    case SettingsAppletInterface::System:
        return QLatin1String("System & Hardware Settings");
    case SettingsAppletInterface::Other:
        return QLatin1String("Other Settings");
    }

    // never will get here, to shut up compiler
    return QLatin1String("Unknown");
}

SettingsAppletInterface* SettingsAppletModel::appletForId(const QString &id) const
{
    for(auto a : m_applets)
    {
        if(a->id() == id)
            return a;
    }

    return nullptr;
}

SettingsAppletInterface::Category SettingsAppletModel::category(const QModelIndex &index) const
{
    return m_applets.at(index.row())->category();
}

SettingsAppletInterface *SettingsAppletModel::applet(const QModelIndex &index) const
{
    return m_applets.at(index.row());
}

void SettingsAppletModel::scanPlugins()
{
#ifdef QT_DEBUG
    auto appletDir = QDir(QCoreApplication::applicationDirPath());
#else
    auto appletDir = QDir(HOLLYWOOD_SETTINGS_PLUGIN_DIR);
#endif
    const auto entryList = appletDir.entryList(QDir::Files);
    for (const QString &fileName : entryList)
    {
        QPluginLoader loader(appletDir.absoluteFilePath(fileName));
        auto applet_obj = loader.instance();
        auto applet = qobject_cast<SettingsAppletInterface*>(applet_obj);
        if (applet && applet->available())
            m_applets.append(applet);
    }
}
