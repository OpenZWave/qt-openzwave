#ifndef QTOZW_PODS_H
#define QTOZW_PODS_H
#include <QObject>

struct NodeStatistics {
        quint32 sentCount;
        quint32 sentFailed;
        quint32 retries;
        quint32 recievedPackets;
        quint32 recievedDupPackets;
        quint32 recievedUnsolicited;
        QString lastSentTimeStamp;
        QString lastReceivedTimeStamp;
        quint32 lastRequestRTT;
        quint32 averageRequestRTT;
        quint32 lastResponseRTT;
        quint32 averageResponseRTT;
        quint8 quality;
        bool extendedTXSupported;
        quint16 txTime;
        quint8 hops;
        QString rssi_1;
        QString rssi_2;
        QString rssi_3;
        QString rssi_4;
        QString rssi_5;
        quint8 ackChannel;
        quint8 lastTXChannel;
        QString routeScheme;
        QString routeUsed;
        QString routeSpeed;
        quint8 routeTries;
        quint8 lastFailedLinkFrom;
        quint8 lastFailedLinkTo;
};

Q_DECLARE_METATYPE(NodeStatistics)

inline QDataStream &operator<<(QDataStream &ds, const NodeStatistics &obj) {
    ds << obj.hops << obj.rssi_1 << obj.rssi_2 << obj.rssi_3 << obj.rssi_4 << obj.rssi_5;
    ds << obj.txTime << obj.quality << obj.retries << obj.routeUsed << obj.sentCount;
    ds << obj.ackChannel << obj.routeSpeed << obj.routeTries << obj.sentFailed;
    ds << obj.routeScheme << obj.lastTXChannel << obj.lastRequestRTT << obj.lastResponseRTT;
    ds << obj.recievedPackets << obj.lastFailedLinkTo << obj.averageRequestRTT;
    ds << obj.lastSentTimeStamp << obj.averageResponseRTT << obj.lastFailedLinkFrom;
    ds << obj.recievedDupPackets << obj.extendedTXSupported << obj.recievedUnsolicited;
    ds << obj.lastReceivedTimeStamp;
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, NodeStatistics &obj) {
#if 0
    ds >> (quint8)obj.hops >> obj.rssi_1 >> obj.rssi_2 >> obj.rssi_3 >> obj.rssi_4 >> obj.rssi_5;
    ds >> obj.txTime << obj.quality << obj.retries << obj.routeUsed << obj.sentCount;
    ds >> obj.ackChannel << obj.routeSpeed << obj.routeTries << obj.sentFailed;
    ds >> obj.routeScheme << obj.lastTXChannel << obj.lastRequestRTT << obj.lastResponseRTT;
    ds >> obj.recievedPackets << obj.lastFailedLinkTo << obj.averageRequestRTT;
    ds >> obj.lastSentTimeStamp << obj.averageResponseRTT << obj.lastFailedLinkFrom;
    ds >> obj.recievedDupPackets << obj.extendedTXSupported << obj.recievedUnsolicited;
    ds >> obj.lastReceivedTimeStamp;
#endif
    return ds;
}


#endif // QTOZW_PODS_H
