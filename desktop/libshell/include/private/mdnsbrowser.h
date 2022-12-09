#ifndef BONJOURSERVICEBROWSER_H
#define BONJOURSERVICEBROWSER_H

#ifdef USE_DNSSD
#include <QtCore/QObject>
#include <dns_sd.h>
#include <QTimer>
#include "mdnsrecord.h"

class QSocketNotifier;
class MDNSBrowser : public QObject
{
    Q_OBJECT
public:
    MDNSBrowser(QObject *parent = 0);
    ~MDNSBrowser();
    inline QList<LSMDNSRecord> currentRecords() const { return _bonjourRecords; }
    inline QString serviceType() const { return browsingType; }
    QList<LSMDNSRecord> _bonjourRecords;
    void setInterval(int interval);

signals:
    void currentBonjourRecordsChanged(const QList<LSMDNSRecord> &list);
    void error(DNSServiceErrorType err);
    void browse(QString dev);
    void finished();

public slots:
    void browseForServiceType(const QString &serviceType);
    void cleanUp();
    void browseForFoundServiceTypes();
    void finishBrowseAttempt();

private slots:
    void bonjourSocketReadyRead();
    void handleError(DNSServiceErrorType err);

private:
    static void DNSSD_API bonjourBrowseReply(DNSServiceRef , DNSServiceFlags flags, quint32,
                                   DNSServiceErrorType errorCode, const char *serviceName,
                                   const char *regType, const char *replyDomain, void *context);
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    QString browsingType;
    int _interval;
};

#endif // USE_DNSSD
#endif // LSMDNSBROWSER_H
