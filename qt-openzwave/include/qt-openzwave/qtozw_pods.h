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
#include <QObject>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>

/** \brief Statistics Relating to Communications with Nodes on your Network
 */
struct NodeStatistics {
        quint32 sentCount; /**< Number of Packets Sent to the Node */
        quint32 sentFailed; /**< Number of Packets that Failed to be acknowledged by the Node or Controller */
        quint32 retries; /**< Number of times we have had to Retry sending packets to the Node */
        quint32 receivedPackets; /**< Number of received Packets from the Node */
        quint32 receivedDupPackets; /**< Number of Duplicate Packets received from the Node */
        quint32 receivedUnsolicited; /**< Number of Unsolicited Packets received from the Node */
        QString lastSentTimeStamp; /**< TimeStamp of the Last time we sent a packet to the Node */
        QString lastReceivedTimeStamp; /**< Timestamp of the last time we received a packet from the Node */
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
};

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

/** \brief Represents a ENUM list for a Single Option
 *
 *  Some of the Options available in the QTOZW_Options class are
 *  in fact a ENUM of a limited set of Possible Values. This simple
 *  Class allows us to represent those value
 */
class OptionList
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


#endif // QTOZW_PODS_H
