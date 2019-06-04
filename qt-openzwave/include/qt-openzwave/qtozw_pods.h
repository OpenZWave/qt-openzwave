#ifndef QTOZW_PODS_H
#define QTOZW_PODS_H
#include <QObject>
#include <QtGlobal>
#include <QDataStream>

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
        quint8 route_1;
        quint8 route_2;
        quint8 route_3;
        quint8 route_4;
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
    ds << static_cast<quint8>(obj.hops);
    ds << static_cast<quint8>(obj.quality);
    ds << static_cast<quint8>(obj.ackChannel);
    ds << static_cast<quint8>(obj.routeTries);
    ds << static_cast<quint8>(obj.lastTXChannel);
    ds << static_cast<quint8>(obj.lastFailedLinkTo);
    ds << static_cast<quint8>(obj.lastFailedLinkFrom);

    ds << static_cast<quint16>(obj.txTime);

    ds << static_cast<quint32>(obj.retries);
    ds << static_cast<quint32>(obj.sentCount);
    ds << static_cast<quint32>(obj.sentFailed);
    ds << static_cast<quint32>(obj.lastRequestRTT);
    ds << static_cast<quint32>(obj.lastResponseRTT);
    ds << static_cast<quint32>(obj.recievedPackets);
    ds << static_cast<quint32>(obj.averageRequestRTT);
    ds << static_cast<quint32>(obj.averageResponseRTT);
    ds << static_cast<quint32>(obj.recievedDupPackets);
    ds << static_cast<quint32>(obj.recievedUnsolicited);

    ds << obj.extendedTXSupported;

    ds << obj.rssi_1;
    ds << obj.rssi_2;
    ds << obj.rssi_3;
    ds << obj.rssi_4;
    ds << obj.rssi_5;
    ds << obj.routeUsed;
    ds << obj.routeSpeed;
    ds << obj.routeScheme;
    ds << obj.lastSentTimeStamp;
    ds << obj.lastReceivedTimeStamp;
    ds << obj.route_1;
    ds << obj.route_2;
    ds << obj.route_3;
    ds << obj.route_4;
    return ds;
}

/* because compilers don't know that a quint8 == unsigned char so we have this stupid marshalling */
inline QDataStream &operator>>(QDataStream &ds, NodeStatistics &obj) {
    quint8 val;
    ds >> val;
    obj.hops = val;
    ds >> val;
    obj.quality = val;
    ds >> val;
    obj.ackChannel = val;
    ds >> val;
    obj.routeTries = val;
    ds >> val;
    obj.lastTXChannel = val;
    ds >> val;
    obj.lastFailedLinkTo = val;
    ds >> val;
    obj.lastFailedLinkFrom = val;

    ds >> obj.txTime;

    ds >> obj.retries;
    ds >> obj.sentCount;
    ds >> obj.sentFailed;
    ds >> obj.lastRequestRTT;
    ds >> obj.lastResponseRTT;
    ds >> obj.recievedPackets;
    ds >> obj.averageRequestRTT;
    ds >> obj.averageResponseRTT;
    ds >> obj.recievedDupPackets;
    ds >> obj.recievedUnsolicited;

    ds >> obj.extendedTXSupported;

    ds >> obj.rssi_1;
    ds >> obj.rssi_2;
    ds >> obj.rssi_3;
    ds >> obj.rssi_4;
    ds >> obj.rssi_5;
    ds >> obj.routeUsed;
    ds >> obj.routeSpeed;
    ds >> obj.routeScheme;
    ds >> obj.lastSentTimeStamp;
    ds >> obj.lastReceivedTimeStamp;
    ds >> obj.route_1;
    ds >> obj.route_2;
    ds >> obj.route_3;
    ds >> obj.route_4;

    return ds;
}


#endif // QTOZW_PODS_H
