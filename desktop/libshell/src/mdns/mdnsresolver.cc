/*
This class was originally implemented in qq23:
http://doc.qt.io/archives/qq/qq23-bonjour.html
Further improved in s1st/exploreBonjour
Copyright (c) 2007, Trenton Schulz
*/

#ifdef USE_DNSSD
#include <QtCore/QSocketNotifier>
#include <QtNetwork/QHostInfo>
#include "mdnsresolver.h"
#include "mdnsrecord.h"

LSMDNSResolver::LSMDNSResolver(QObject *parent)
    : QObject(parent), dnssref(0), bonjourSocket(0), bonjourPort(-1)
{
}

LSMDNSResolver::~LSMDNSResolver()
{
    cleanupResolve();
}

void LSMDNSResolver::cleanupResolve()
{
    if (dnssref) {
        DNSServiceRefDeallocate(dnssref);
        dnssref = 0;
        delete bonjourSocket;
        bonjourPort = -1;
    }
    emit cleanedUp();
}

void LSMDNSResolver::resolveBonjourRecord(const LSMDNSRecord &record)
{
    if (dnssref) {
        qWarning("resolve in process, aborting");
        return;
    }
    qDebug() << "record: " << record.serviceName << record.registeredType << record.replyDomain;
    DNSServiceErrorType err = DNSServiceResolve(&dnssref, 0, 0,
                                                record.serviceName.toUtf8().constData(),
                                                record.registeredType.toUtf8().constData(),
                                                record.replyDomain.toUtf8().constData(),
                                                (DNSServiceResolveReply)bonjourResolveReply, this);
    if (err != kDNSServiceErr_NoError) {
        qDebug() << "kDNSServiceErr: " << err;
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

void LSMDNSResolver::bonjourSocketReadyRead()
{
    DNSServiceErrorType err = DNSServiceProcessResult(dnssref);
    if (err != kDNSServiceErr_NoError)
        emit error(err);
}


void LSMDNSResolver::bonjourResolveReply(DNSServiceRef, DNSServiceFlags ,
                                    quint32 , DNSServiceErrorType errorCode,
                                    const char *, const char *hosttarget, quint16 port,
                                    quint16 , const char *, void *context)
{
    LSMDNSResolver *serviceResolver = static_cast<LSMDNSResolver *>(context);
    if (errorCode != kDNSServiceErr_NoError) {
        emit serviceResolver->error(errorCode);
        return;
    }
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        {
            port =  0 | ((port & 0x00ff) << 8) | ((port & 0xff00) >> 8);
        }
#endif
    serviceResolver->bonjourPort = port;
    QHostInfo::lookupHost(QString::fromUtf8(hosttarget),
                          serviceResolver, SLOT(finishConnect(const QHostInfo &)));
}

void LSMDNSResolver::finishConnect(const QHostInfo &hostInfo)
{
    emit bonjourRecordResolved(hostInfo);
    QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void LSMDNSResolver::handleError(DNSServiceErrorType err)
{
    qDebug() << "Resolver -- handling following error: " << err;
    cleanupResolve();
}
#endif // USE_DNSSD
