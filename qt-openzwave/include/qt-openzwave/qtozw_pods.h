//-----------------------------------------------------------------------------
//
//	qtozw_pods.h
//
//	Structure Definitions for QT-OpenZWave Wrapper
//
//	Copyright (c) 2019 Justin Hammond <Justin@dynam.ac>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of QT-OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#ifndef QTOZW_PODS_H
#define QTOZW_PODS_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>

#if defined(QTOPENZWAVE_LIBRARY)
#define QTOPENZWAVESHARED_EXPORT Q_DECL_EXPORT
#else
#define QTOPENZWAVESHARED_EXPORT Q_DECL_IMPORT
#define OPENZWAVE_USEDLL 1
#endif


/** \brief Statistics Relating to Communications with Nodes on your Network
 */
typedef struct NodeStatistics {
        quint32 sentCount; /**< Number of Packets Sent to the Node */
        quint32 sentFailed; /**< Number of Packets that Failed to be acknowledged by the Node or Controller */
        quint32 retries; /**< Number of times we have had to Retry sending packets to the Node */
        quint32 receivedPackets; /**< Number of received Packets from the Node */
        quint32 receivedDupPackets; /**< Number of Duplicate Packets received from the Node */
        quint32 receivedUnsolicited; /**< Number of Unsolicited Packets received from the Node */
        QDateTime lastSentTimeStamp; /**< TimeStamp of the Last time we sent a packet to the Node */
        QDateTime lastReceivedTimeStamp; /**< Timestamp of the last time we received a packet from the Node */
        quint32 lastRequestRTT; /**<  Last Round-Trip Time when we made a request to the Node */
        quint32 averageRequestRTT; /**< Average Round-Trip Time when we make requests to a Node */
        quint32 lastResponseRTT; /**< Last Round-Trip Time when we got a Response from a Node */
        quint32 averageResponseRTT; /**< Average Round-Trip Time when got a Response from a Node */
        quint8 quality; /**< The Quality of the Signal from the Node, as Reported by the Controller */
        bool extendedTXSupported; /**< If these statistics support Extended TX Reporting (Controller Dependent) */
        quint16 txTime; /**< The Time it took to Transmit the last packet to the Node */
        quint8 hops; /**< The Number of hops the packet traversed to reach the node */
        QString rssi_1; /**< The RSSI Strength of the first hop */
        QString rssi_2; /**< The RSSI Strength of the second hop */
        QString rssi_3; /**< The RSSI Strength of the third hop */
        QString rssi_4; /**< The RSSI Strength of the fourth hop */
        QString rssi_5; /**< The RSSI Strength of the final hop */
        quint8 route_1; /**< The NodeId of the First Hop */
        quint8 route_2; /**< The NodeId of the Second Hop */
        quint8 route_3; /**< The NodeId of the third Hop */
        quint8 route_4; /**< The NodeId of the Fourth Hop */
        quint8 ackChannel; /**< The Channel that recieved the ACK From the Node */
        quint8 lastTXChannel; /**< The last Channel we used to communicate with the Node */
        QString routeScheme; /**< How the Route was calculated when we last communicated with the Node */
        QString routeUsed; /**< The Route last used to communicate with the Node */
        QString routeSpeed; /**< The Speed that was used when we last communicated with the node */
        quint8 routeTries; /**< The Number of attempts the Controller made to route the packet to the Node */
        quint8 lastFailedLinkFrom; /**< The Last Failed Link From */
        quint8 lastFailedLinkTo; /**< The Last Failed Link To */
} NodeStatistics;

/** \cond DO_NOT_DOCUMENT
 */
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
    ds << static_cast<quint32>(obj.receivedPackets);
    ds << static_cast<quint32>(obj.averageRequestRTT);
    ds << static_cast<quint32>(obj.averageResponseRTT);
    ds << static_cast<quint32>(obj.receivedDupPackets);
    ds << static_cast<quint32>(obj.receivedUnsolicited);

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
    ds >> obj.receivedPackets;
    ds >> obj.averageRequestRTT;
    ds >> obj.averageResponseRTT;
    ds >> obj.receivedDupPackets;
    ds >> obj.receivedUnsolicited;

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

/** \endcond
 */


/** \brief Statistics Relating to Communications with the Network
 */
typedef struct DriverStatistics {
    quint32 	m_SOFCnt;
    quint32 	m_ACKWaiting;
    quint32 	m_readAborts;
    quint32 	m_badChecksum;
    quint32 	m_readCnt;
    quint32 	m_writeCnt;
    quint32 	m_CANCnt;
    quint32 	m_NAKCnt;
    quint32 	m_ACKCnt;
    quint32 	m_OOFCnt;
    quint32 	m_dropped;
    quint32 	m_retries;
    quint32 	m_callbacks;
    quint32 	m_badroutes;
    quint32 	m_noack;
    quint32 	m_netbusy;
    quint32 	m_notidle;
    quint32 	m_txverified;
    quint32 	m_nondelivery;
    quint32 	m_routedbusy;
    quint32 	m_broadcastReadCnt;
    quint32 	m_broadcastWriteCnt;
} DriverStatistics;

Q_DECLARE_METATYPE(DriverStatistics)

inline QDataStream &operator<<(QDataStream &ds, const DriverStatistics &obj) {
    ds << static_cast<quint32>(obj.m_SOFCnt);
    ds << static_cast<quint32>(obj.m_ACKWaiting);
    ds << static_cast<quint32>(obj.m_readAborts);
    ds << static_cast<quint32>(obj.m_badChecksum);
    ds << static_cast<quint32>(obj.m_readCnt);
    ds << static_cast<quint32>(obj.m_writeCnt);
    ds << static_cast<quint32>(obj.m_CANCnt);
    ds << static_cast<quint32>(obj.m_NAKCnt);
    ds << static_cast<quint32>(obj.m_ACKCnt);
    ds << static_cast<quint32>(obj.m_OOFCnt);
    ds << static_cast<quint32>(obj.m_dropped);
    ds << static_cast<quint32>(obj.m_retries);
    ds << static_cast<quint32>(obj.m_callbacks);
    ds << static_cast<quint32>(obj.m_badroutes);
    ds << static_cast<quint32>(obj.m_noack);
    ds << static_cast<quint32>(obj.m_netbusy);
    ds << static_cast<quint32>(obj.m_notidle);
    ds << static_cast<quint32>(obj.m_txverified);
    ds << static_cast<quint32>(obj.m_nondelivery);
    ds << static_cast<quint32>(obj.m_routedbusy);
    ds << static_cast<quint32>(obj.m_broadcastReadCnt);
    ds << static_cast<quint32>(obj.m_broadcastWriteCnt);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, DriverStatistics &obj) {
    ds >> obj.m_SOFCnt;
    ds >> obj.m_ACKWaiting;
    ds >> obj.m_readAborts;
    ds >> obj.m_badChecksum;
    ds >> obj.m_readCnt;
    ds >> obj.m_writeCnt;
    ds >> obj.m_CANCnt;
    ds >> obj.m_NAKCnt;
    ds >> obj.m_ACKCnt;
    ds >> obj.m_OOFCnt;
    ds >> obj.m_dropped;
    ds >> obj.m_retries;
    ds >> obj.m_callbacks;
    ds >> obj.m_badroutes;
    ds >> obj.m_noack;
    ds >> obj.m_netbusy;
    ds >> obj.m_notidle;
    ds >> obj.m_txverified;
    ds >> obj.m_nondelivery;
    ds >> obj.m_routedbusy;
    ds >> obj.m_broadcastReadCnt;
    ds >> obj.m_broadcastWriteCnt;
    return ds;
}
/** \brief Represents a ENUM list for a Single Option
 *
 *  Some of the Options available in the QTOZW_Options class are
 *  in fact a ENUM of a limited set of Possible Values. This simple
 *  Class allows us to represent those value
 */
class QTOPENZWAVESHARED_EXPORT OptionList
{
    Q_GADGET
    friend QDataStream &operator<<(QDataStream &ds, const OptionList &obj);
    friend QDataStream &operator>>(QDataStream &ds, OptionList &obj);
public:
    /** \brief Constructor
     */
    OptionList();
    /** \brief Deconstructor
     */
    ~OptionList();
    /** \brief Comparision Operator
     */
    bool operator!=(OptionList &c2);
    /** \brief Provide a List of Possible Values
     *
     *  This is the List of Possible Values this Option accepts
     *  Applications should not call this function, as it will be populated
     *  by QTOZW_Options_Internal based on what is available in OpenZWave
     *  \param list A QStringList of values
     */
    void setEnums(QStringList list);
    /** \brief Get a List of Possible values for this Option
     *
     *  Retreive a list of possible values for this Option.
     *  Can be used to populate a QComboBox for example
     *  \return A QStringList of possible values
     */
    QStringList getEnums();
    /** \brief Get the Selected Option
     *
     *  Retrives the Currently Selected Option as a String
     *  \return A String representing the current value
     */
    QString getSelectedName();
    /** \brief Get the Index of the Selected value
     *
     *  Retrieves the Position of the Currently Selected value. The Position is based upon the order
     *  that the values were passed in via setEnums
     *  \return the Index of the currently selected value
     */
    int getSelected();
    /** \brief Set the Selected value based upon the position
     *
     *  Sets the currently selected value based to the index of the value
     *  passed in via the setEnums call
     *  \param index the position of the Value to Select
     *  \return bool if the Value was successfully stored.
     */
    bool setSelected(int index);
    /** \brief Set the Selected value based upon the string
     *
     *  Sets the currently selected value to value, as long as it
     *  exists in the List passed in via setEnums
     *  \param value the String that you wish to set the value to
     *  \return bool if the value was successfully stored.
     */
    bool setSelected(QString value);
private:
    QStringList enumnames;
    int selected;
};

/** \cond DO_NOT_DOCUMENT
 */

Q_DECLARE_METATYPE(OptionList)

inline QDataStream &operator<<(QDataStream &ds, const OptionList &obj) {
    ds << obj.enumnames;
    ds << obj.selected;
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, OptionList &obj) {
    ds >> obj.enumnames;
    ds >> obj.selected;
    return ds;
}
/** \endcond
 */

    class QTOPENZWAVESHARED_EXPORT NotificationTypes : public QObject {
        Q_OBJECT;
        public:
        enum QTOZW_Notification_Code {
            Notification_Code_MsgComplete = 0,
            Notification_Code_MsgTimeout = 1,
            Notification_Code_NoOperation = 2,
            Notification_Code_NodeAwake = 3,
            Notification_Code_NodeAsleep = 4,
            Notification_Code_NodeDead = 5,
            Notification_Code_NodeAlive = 6,
            Notification_Code_count 
        };
        Q_ENUM(QTOZW_Notification_Code);

        enum QTOZW_Notification_User {
            Notification_User_None = 0,
            Notification_User_ConfigOutOfDate = 1,
            Notification_User_MFSOutOfDate = 2,
            Notification_User_ConfigFileDownloadFailed = 3,
            Notification_User_DNSError = 4,
            Notification_User_NodeReloadRequired = 5,
            Notification_User_UnsupportedController = 6,
            Notification_User_ApplicationStatus_Retry = 7,
            Notification_User_ApplicationStatus_Queued = 8,
            Notification_User_ApplicationStatus_Rejected = 9,
            Notification_User_count
        };
        Q_ENUM(QTOZW_Notification_User);

        enum QTOZW_Notification_Controller_Error {
            Ctrl_Error_None = 0,
            Ctrl_Error_ButtonNotFound = 1,
            Ctrl_Error_NodeNotFound = 2,
            Ctrl_Error_NotBridge = 3,
            Ctrl_Error_NotSUC = 4,
            Ctrl_Error_NotSecondary = 5,
            Ctrl_Error_NotPrimary = 6,
            Ctrl_Error_IsPrimary = 7,
            Ctrl_Error_NotFound = 8,
            Ctrl_Error_Busy = 9,
            Ctrl_Error_Failed = 10,
            Ctrl_Error_Disabled = 11,
            Ctrl_Error_OverFlow = 12,
            Ctrl_Error_count
        };
        Q_ENUM(QTOZW_Notification_Controller_Error);
        
        enum QTOZW_Notification_Controller_State {
            Ctrl_State_Normal = 0,
            Ctrl_State_Starting = 1,
            Ctrl_State_Cancel = 2,
            Ctrl_State_Error = 3,
            Ctrl_State_Waiting = 4,
            Ctrl_State_Sleeping = 5,
            Ctrl_State_InProgress = 6,
            Ctrl_State_Completed = 7,
            Ctrl_State_Failed = 8,
            Ctrl_State_NodeOk = 9,
            Ctrl_State_NodeFailed = 10,
            Ctrl_State_count
        };
        Q_ENUM(QTOZW_Notification_Controller_State);

        enum QTOZW_Notification_Controller_Cmd {
            Ctrl_Cmd_None = 0,
            Ctrl_Cmd_AddNode = 1,
            Ctrl_Cmd_CreateNewPrimary = 2,
            Ctrl_Cmd_ReceiveConfiguration = 3,
            Ctrl_Cmd_RemoveNode = 4,
            Ctrl_Cmd_RemoveFailedNode = 5,
            Ctrl_Cmd_HasNodeFailed = 6,
            Ctrl_Cmd_ReplaceFailedNode = 7,
            Ctrl_Cmd_TransferPrimaryRole = 8,
            Ctrl_Cmd_RequestNetworkUpdate = 9,
            Ctrl_Cmd_RequestNodeNeighborUpdate = 10,
            Ctrl_Cmd_AssignReturnRoute = 11,
            Ctrl_Cmd_DeleteAllReturnRoute = 12,
            Ctrl_Cmd_SendNodeInformation = 13,
            Ctrl_Cmd_ReplicationSend = 14,
            Ctrl_Cmd_CreateButton = 15,
            Ctrl_Cmd_DeleteButton = 16,
            Ctrl_Cmd_count
        };
        Q_ENUM(QTOZW_Notification_Controller_Cmd);
    };

Q_DECLARE_METATYPE(NotificationTypes::QTOZW_Notification_Code)


inline QDataStream &operator<<(QDataStream &ds, const NotificationTypes::QTOZW_Notification_Code &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, NotificationTypes::QTOZW_Notification_Code &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<NotificationTypes::QTOZW_Notification_Code>(val);
    return ds;
}

Q_DECLARE_METATYPE(NotificationTypes::QTOZW_Notification_User)

inline QDataStream &operator<<(QDataStream &ds, const NotificationTypes::QTOZW_Notification_User &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, NotificationTypes::QTOZW_Notification_User &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<NotificationTypes::QTOZW_Notification_User>(val);
    return ds;
}

Q_DECLARE_METATYPE(NotificationTypes::QTOZW_Notification_Controller_Error)


inline QDataStream &operator<<(QDataStream &ds, const NotificationTypes::QTOZW_Notification_Controller_Error &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, NotificationTypes::QTOZW_Notification_Controller_Error &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<NotificationTypes::QTOZW_Notification_Controller_Error>(val);
    return ds;
}

Q_DECLARE_METATYPE(NotificationTypes::QTOZW_Notification_Controller_State)

inline QDataStream &operator<<(QDataStream &ds, const NotificationTypes::QTOZW_Notification_Controller_State &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, NotificationTypes::QTOZW_Notification_Controller_State &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<NotificationTypes::QTOZW_Notification_Controller_State>(val);
    return ds;
}

Q_DECLARE_METATYPE(NotificationTypes::QTOZW_Notification_Controller_Cmd)

inline QDataStream &operator<<(QDataStream &ds, const NotificationTypes::QTOZW_Notification_Controller_Cmd &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, NotificationTypes::QTOZW_Notification_Controller_Cmd &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<NotificationTypes::QTOZW_Notification_Controller_Cmd>(val);
    return ds;
}

class QTOPENZWAVESHARED_EXPORT ValueTypes : public QObject {
    Q_OBJECT;
    public:
    enum valueGenre {
        Basic = 0,
        User,
        Config,
        System,
        InvalidGenre = 255
    };
    Q_ENUM(valueGenre);
    enum valueType {
        Bool = 0,
        Byte,
        Decimal,
        Int,
        List,
        Schedule,
        Short,
        String,
        Button,
        Raw,
        BitSet,
        InvalidType = 255
    };
    Q_ENUM(valueType);
};

Q_DECLARE_METATYPE(ValueTypes::valueGenre)

inline QDataStream &operator<<(QDataStream &ds, const ValueTypes::valueGenre &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, ValueTypes::valueGenre &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<ValueTypes::valueGenre>(val);
    return ds;
}

Q_DECLARE_METATYPE(ValueTypes::valueType)

inline QDataStream &operator<<(QDataStream &ds, const ValueTypes::valueType &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, ValueTypes::valueType &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<ValueTypes::valueType>(val);
    return ds;
}

class QTOPENZWAVESHARED_EXPORT LogLevels : public QObject {
    Q_OBJECT;
    public:
        enum Level {
            Always,
            Fatal,
            Error,
            Warning,
            Alert,
            Info,
            Detail,
            Debug,
            StreamDetail,
            Internal,
            LogLevelCount
        };
        Q_ENUM(Level)
};

Q_DECLARE_METATYPE(LogLevels::Level)

inline QDataStream &operator<<(QDataStream &ds, const LogLevels::Level &obj) {
    ds << static_cast<qint32>(obj);
    return ds;
}

inline QDataStream &operator>>(QDataStream &ds, LogLevels::Level &obj) {
    qint32 val;
    ds >> val;
    obj = static_cast<LogLevels::Level>(val);
    return ds;
}
class QTOPENZWAVESHARED_EXPORT ConnectionType : public QObject {
    Q_OBJECT
    public:
        enum Type {
            Local,
            Remote,
            Invalid
        };
        Q_ENUM(Type)
};


#endif // QTOZW_PODS_H
