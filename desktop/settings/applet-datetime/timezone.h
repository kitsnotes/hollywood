/* Hollywood Operating System
 * System Settings - Date & Time Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-FileCopyrightText: 2019 Adriaan de Groot <groot@kde.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file was originally adopted from Calamares
 */

#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QVariant>

class TranslatableString
{
public:
    TranslatableString() {}
    explicit TranslatableString( const char* s1 );
    explicit TranslatableString( const QString& s );
    TranslatableString( TranslatableString&& t );
    TranslatableString( const TranslatableString& );
    virtual ~TranslatableString();

    virtual QString tr() const = 0;
    QString key() const { return m_key; }

    bool operator==( const TranslatableString& other ) const { return m_key == other.m_key; }
    bool operator<( const TranslatableString& other ) const { return m_key < other.m_key; }

protected:
    char* m_human = nullptr;
    QString m_key;
};

class RegionData;
class TimeZoneData;
using RegionVector = QVector< RegionData* >;
using ZoneVector = QVector< TimeZoneData* >;
class Private : public QObject
{
    Q_OBJECT
public:
    Private();
    RegionVector m_regions;
    ZoneVector m_zones;  ///< The official timezones and locations
    ZoneVector m_altZones;  ///< Extra locations for zones
};

class RegionalZonesModel;
class ZonesModel;

class TimeZoneData : public QObject, TranslatableString
{
    friend class RegionalZonesModel;
    friend class ZonesModel;

    Q_OBJECT

    Q_PROPERTY( QString region READ region CONSTANT )
    Q_PROPERTY( QString zone READ zone CONSTANT )
    Q_PROPERTY( QString name READ tr CONSTANT )
    Q_PROPERTY( QString countryCode READ country CONSTANT )

public:
    TimeZoneData( const QString& region,
                  const QString& zone,
                  const QString& country,
                  double latitude,
                  double longitude );
    TimeZoneData( const TimeZoneData& ) = delete;
    TimeZoneData( TimeZoneData&& ) = delete;

    QString tr() const override;

    QString region() const { return m_region; }
    QString zone() const { return key(); }

    QString country() const { return m_country; }
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }

private:
    QString m_region;
    QString m_country;
    double m_latitude;
    double m_longitude;
};

class RegionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        NameRole = Qt::DisplayRole,
        KeyRole = Qt::UserRole  // So that currentData() will get the key
    };

    RegionsModel( QObject* parent = nullptr );
    ~RegionsModel() override;

    int rowCount( const QModelIndex& parent ) const override;
    QVariant data( const QModelIndex& index, int role ) const override;

    QHash< int, QByteArray > roleNames() const override;

public Q_SLOTS:
    QString tr( const QString& region ) const;

private:
    Private* m_private;
};

class ZonesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        NameRole = Qt::DisplayRole,
        KeyRole = Qt::UserRole,  // So that currentData() will get the key
        RegionRole = Qt::UserRole + 1
    };

    ZonesModel( QObject* parent = nullptr );
    ~ZonesModel() override;

    int rowCount( const QModelIndex& parent ) const override;
    QVariant data( const QModelIndex& index, int role ) const override;

    QHash< int, QByteArray > roleNames() const override;

    class Iterator
    {
        friend class ZonesModel;
        Iterator( const Private* m )
            : m_index( 0 )
            , m_p( m )
        {
        }

    public:
        operator bool() const;
        void operator++() { ++m_index; }
        const TimeZoneData* operator*() const;
        int index() const { return m_index; }

    private:
        int m_index;
        const Private* m_p;
    };

    Iterator begin() const { return Iterator( m_private ); }

    const TimeZoneData* findtz( const std::function< double( const TimeZoneData* ) >& distanceFunc ) const;

public Q_SLOTS:
    const TimeZoneData* findtz( const QString& region, const QString& zone ) const;
    const TimeZoneData* findtz( double latitude, double longitude ) const;
    QObject* lookup( double latitude, double longitude ) const;

private:
    Private* m_private;
};

class RegionalZonesModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY( QString region READ region WRITE setRegion NOTIFY regionChanged )

public:
    RegionalZonesModel( ZonesModel* source, QObject* parent = nullptr );
    ~RegionalZonesModel() override;

    bool filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const override;

    QString region() const { return m_region; }

public Q_SLOTS:
    void setRegion( const QString& r );

signals:
    void regionChanged( const QString& );

private:
    Private* m_private;
    QString m_region;
};

#endif  // LOCALE_TIMEZONE_H
