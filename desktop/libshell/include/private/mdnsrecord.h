#ifndef MDNSRECORD_H
#define MDNSRECORD_H
#ifdef USE_DNSSD
#include <QtCore/QMetaType>
#include <QtCore/QString>

class LSMDNSRecord
{
public:
    LSMDNSRecord() {}
    LSMDNSRecord(const QString &name, const QString &regType, const QString &domain)
        : serviceName(name), registeredType(regType), replyDomain(domain)
    {}
    LSMDNSRecord(const char *name, const char *regType, const char *domain)
    {
        serviceName = QString::fromUtf8(name);
        registeredType = QString::fromUtf8(regType);
        replyDomain = QString::fromUtf8(domain);
    }
    QString serviceName;
    QString registeredType;
    QString replyDomain;
    bool operator==(const LSMDNSRecord &other) const {
        return serviceName == other.serviceName
               && registeredType == other.registeredType
               && replyDomain == other.replyDomain;
    }
};

Q_DECLARE_METATYPE(LSMDNSRecord)
#endif // USE_DNSSD
#endif // MDNSRECORD_H
