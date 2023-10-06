#ifdef USE_DNSSD
#include "mdnsbrowser.h"
#include <QtCore/QSocketNotifier>
#include <QDebug>

MDNSBrowser::MDNSBrowser(QObject *parent)
    : QObject(parent), dnssref(0), bonjourSocket(0)
{
    connect(this, SIGNAL(browse(QString)), this, SLOT(browseForServiceType(QString)));
}

MDNSBrowser::~MDNSBrowser()
{
    if (dnssref) {
        DNSServiceRefDeallocate(dnssref);
        dnssref = 0;
    }
}

void MDNSBrowser::browseForServiceType(const QString &serviceType)
{
    if(dnssref)
    {
        qDebug() << "dnssref existing - error - cleanup needed";
    }
    qDebug() << "browsing for:" << serviceType;
    DNSServiceErrorType err = DNSServiceBrowse(&dnssref, 0, 0, serviceType.toUtf8().constData(), 0,
                                               bonjourBrowseReply, this);
    if (err != kDNSServiceErr_NoError) {
        emit error(err);
    } else {
        int sockfd = DNSServiceRefSockFD(dnssref);
        if (sockfd == -1) {
            emit error(kDNSServiceErr_Invalid);
        } else {
            bonjourSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(bonjourSocket, SIGNAL(activated(int)), this, SLOT(bonjourSocketReadyRead()));
        }
    }
}

void MDNSBrowser::browseForFoundServiceTypes()
{
    if(_bonjourRecords.isEmpty())
        emit finished();

    for(auto b : _bonjourRecords)
        qDebug() << b.registeredType << b.replyDomain << b.serviceName;

    QString type = _bonjourRecords.at(0).registeredType;
    QString service = _bonjourRecords.at(0).serviceName;
    if(type.endsWith("."))
        type.chop(1);

    service.append(".").append(type);
    QByteArray ba;
    ba.append(service.toUtf8());
    if(type == "_tcp" || type == "_udp" || type == "_dns-sd._udp" ||
            (type == "_apple-mobdev2._tcp" && service == "_sub"))
    {
        QTimer::singleShot(_interval, this, SLOT(finishBrowseAttempt()));
        qDebug() << "type, sub: " << type << service;
        browseForServiceType(QLatin1String(ba));
    }
    else
    {
        qDebug() << ">>Aborting: type, sub: " << type << service;
        emit finished();
    }
}

void MDNSBrowser::finishBrowseAttempt()
{
    _bonjourRecords.removeFirst();
    cleanUp();
    browseForFoundServiceTypes();
}

void MDNSBrowser::cleanUp()
{
    if(dnssref)
    {
        DNSServiceRefDeallocate(dnssref);
        dnssref = 0;
        delete bonjourSocket;
    }
}

void MDNSBrowser::bonjourSocketReadyRead()
{
    DNSServiceErrorType err = DNSServiceProcessResult(dnssref);
    if (err != kDNSServiceErr_NoError)
        emit error(err);
}

void MDNSBrowser::handleError(DNSServiceErrorType err)
{
    qDebug() << "Browser -- handling following error: " << err;
}

void MDNSBrowser::bonjourBrowseReply(DNSServiceRef , DNSServiceFlags flags,
                                               quint32 , DNSServiceErrorType errorCode,
                                               const char *serviceName, const char *regType,
                                               const char *replyDomain, void *context)
{
    MDNSBrowser *serviceBrowser = static_cast<MDNSBrowser *>(context);
    if (errorCode != kDNSServiceErr_NoError)
        emit serviceBrowser->error(errorCode);
    else
    {
        LSMDNSRecord bonjourRecord(serviceName, regType, replyDomain);
        if (flags & kDNSServiceFlagsAdd) {
            if (!serviceBrowser->_bonjourRecords.contains(bonjourRecord))
                serviceBrowser->_bonjourRecords.append(bonjourRecord);
        } else {
            serviceBrowser->_bonjourRecords.removeAll(bonjourRecord);
        }
        if (!(flags & kDNSServiceFlagsMoreComing)) {
            emit serviceBrowser->currentBonjourRecordsChanged(serviceBrowser->_bonjourRecords);
        }
    }
}

void MDNSBrowser::setInterval(int interval)
{
    _interval = interval;
}

#endif // USE_DNSSD
