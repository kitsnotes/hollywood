#pragma once

#ifdef USE_DNSSD
#include <QtCore/QObject>
#include <dns_sd.h>

class QSocketNotifier;
class QHostInfo;
class LSMDNSRecord;

class LSMDNSResolver : public QObject
{
    Q_OBJECT
public:
    LSMDNSResolver(QObject *parent);
    ~LSMDNSResolver();

    void resolveBonjourRecord(const LSMDNSRecord &record);

signals:
    void bonjourRecordResolved(const QHostInfo &hostInfo);
    void error(DNSServiceErrorType error);
    void cleanedUp();

private slots:
    void cleanupResolve();
    void bonjourSocketReadyRead();
    void finishConnect(const QHostInfo &hostInfo);
    void handleError(DNSServiceErrorType err);

private:
    static void DNSSD_API bonjourResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags,
                                    quint32 interfaceIndex, DNSServiceErrorType errorCode,
                                    const char *fullName, const char *hosttarget, quint16 port,
                                    quint16 txtLen, const char *txtRecord, void *context);

    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    int bonjourPort;
};
#endif // USE_DNSSD
