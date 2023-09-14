#ifndef UPOWERINTERFACE_H
#define UPOWERINTERFACE_H

#include <QDBusAbstractInterface>
#include <QObject>
#include <QVariant>

class UPowerInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    UPowerInterface(QObject *parent = nullptr);
    ~UPowerInterface() = default;
    QList<QDBusObjectPath> enumerateDevices();
};

class UPowerDeviceInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    explicit UPowerDeviceInterface(const QString &path, QObject *parent = nullptr);
    ~UPowerDeviceInterface();

    quint16 type();
    quint16 state();
    double percent();
    qint64 timeUntilFull();
    quint64 timeUntilEmpty();
    QString displayIcon();
signals:
    void changed();
private slots:
    void updateProperties();
private:
    QDBusInterface *m_interface;
    QVariantMap m_props;
    QString m_path;
};

#endif // UPOWERINTERFACE_H
