//-----------------------------------------------------------------------------
//
//	qtozwmanager_p.h
//
//	QT-OpenZWave Manager - Internal Class to interface between OZW and the QT
//  Wrapper
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

#ifndef QTOZWMANAGER_P_H
#define QTOZWMANAGER_P_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QUrl>
#include "qtozw_logging.h"
#include "rep_qtozwmanager_source.h"
#include "rep_qtozwmanager_replica.h"
#include "qtozwnotification.h"
#include "qt-openzwave/qtozwmanager.h"
#include "qtozwoptions_p.h"
#include "qtozwnodemodel_p.h"
#include "qtozwvalueidmodel_p.h"
#include "qtozwassociationmodel_p.h"
#include "qtozwlog_p.h"

/* OZW Includes */
#include "Manager.h"
#include "Notification.h"
#include "platform/Log.h"
#include "OZWException.h"
#include "command_classes/CommandClasses.h"



class QTOZWManager_Internal : public QTOZWManagerSimpleSource
{
    Q_OBJECT
public:
    friend class OZWNotification;
    QTOZWManager_Internal(QObject *parent = nullptr);

    QTOZW_Nodes *getNodeModel();
    QTOZW_ValueIds *getValueModel();
    QTOZW_Associations *getAssociationModel();
    QTOZWOptions_Internal *getOptions();
    QTOZW_Log_Internal *getLogModel();
    bool setLogModel(QTOZW_Log_Internal *);

public Q_SLOTS:
    bool open(QString serialPort);
    bool close();
    bool refreshNodeInfo(quint8 _node);
    bool requestNodeState(quint8 _node);
    bool requestNodeDynamic(quint8 _node);

    bool setConfigParam(quint8 _node, quint8 _param, int32_t _value, quint8 const _size);
    void requestConfigParam(quint8 _node, quint8 _param);
    void requestAllConfigParam(quint8 _node);

    void softResetController();
    void hardResetController();

    bool cancelControllerCommand();

    void testNetworkNode(quint8 _node, quint32 const _count);
    void testNetwork(quint32 const _count);
    void healNetworkNode(quint8 _node, bool _doRR);
    void healNetwork(bool _doRR);
    bool addNode(bool _doSecure);
    bool removeNode();
    bool removeFailedNode(quint8 _node);
    bool hasNodeFailed(quint8 _node);
    bool requestNodeNeighborUpdate(quint8 _node);
    bool assignReturnRoute(quint8 _node);
    bool deleteAllReturnRoute(quint8 _node);
    bool sendNodeInformation(quint8 _node);
    bool replaceFailedNode(quint8 _node);
    bool requestNetworkUpdate(quint8 _node);
    QString GetMetaData(quint8 _node, QTOZWManagerSource::QTOZWMetaDataField _field);
    QByteArray GetMetaDataProductPic(quint8 _node);

    QString GetNodeQueryStage(quint8 const node);
    NodeStatistics GetNodeStatistics(quint8 const node);
    DriverStatistics GetDriverStatistics();

    QVector<quint8> GetNodeNeighbors(quint8 const node);
    
    bool IsNodeFailed(quint8 const _node);

    bool checkLatestConfigFileRevision(quint8 const _node);
    bool checkLatestMFSRevision();
    bool downloadLatestConfigFileRevision(quint8 const _node);
    bool downloadLatestMFSRevision();

    QString getCommandClassString(quint8 const _cc);
    quint8 getCommandClassVersion(quint8 const _node, quint8 const _cc);

    QString getVersionAsString();
    QString getVersionLongAsString();
    quint8 getControllerNodeId();
    quint8 getSucNodeId();
    bool isPrimaryController();
    bool isStaticUpdateController();
    bool isBridgeController();
    bool hasExtendedTXStatus();
    QString getLibraryVersion();
    QString getLibraryTypeName();
    quint32 getSendQueueCount();
    QString getControllerPath();

    qint32 getPollInterval();

    void setPollInterval(qint32 interval, bool intervalBetweenPolls);
    void syncroniseNodeNeighbors(quint8 node);

    bool refreshValue(quint64);

    bool AddAssociation (quint8 const _nodeId, quint8 const _groupIdx, QString const target);
    bool RemoveAssociation (quint8 const _nodeId, quint8 const _groupIdx, QString const target);

    bool enablePoll(quint64 vidKey, quint8 intensity);
    bool disablePoll(quint64 vidKey);

    QString getInstanceLabel(quint64 vidKey);
    QString getValueLabel(quint64 vidKey);
    QString getValueUnits(quint64 vidKey);
    QString getValueHelp(quint64 vidKey);
    qint32 getValueMin(quint64 vidKey);
    qint32 getValueMax(quint64 vidKey);
    bool isValueReadOnly(quint64 vidKey);
    bool isValueWriteOnly(quint64 vidKey);
    bool isValueSet(quint64 vidKey);
    bool isValuePolled(quint64 vidKey);
    bool isValueValid(quint64 vidKey);

    /* these slots are called from our OZWNotification Class. Applications should not call them */
    void pvt_valueAdded(quint64 vidKey);
    void pvt_valueRemoved(quint64 vidKey);
    void pvt_valueChanged(quint64 vidKey);
    void pvt_valueRefreshed(quint64 vidKey);
    void pvt_valuePollingEnabled(quint64 vidKey);
    void pvt_valuePollingDisabled(quint64 vidKey);
    void pvt_nodeGroupChanged(quint8 node, quint8 group);
    void pvt_nodeNew(quint8 node);
    void pvt_nodeAdded(quint8 node);
    void pvt_nodeRemoved(quint8 node);
    void pvt_nodeReset(quint8 node);
    void pvt_nodeNaming(quint8 node);
    void pvt_nodeEvent(quint8 node, quint8 event);
    void pvt_nodeProtocolInfo(quint8 node);
    void pvt_nodeEssentialNodeQueriesComplete(quint8 node);
    void pvt_nodeQueriesComplete(quint8 node);
    void pvt_driverReady(quint32 homeID);
    void pvt_driverFailed(quint32 homeID);
    void pvt_driverReset(quint32 homeID);
    void pvt_driverRemoved(quint32 homeID);
    void pvt_driverAllNodesQueriedSomeDead();
    void pvt_driverAllNodesQueried();
    void pvt_driverAwakeNodesQueried();
    void pvt_controllerCommand(quint8 node, quint32 cmd, quint32 state, quint32 error);
    void pvt_ozwNotification(quint8 node, quint32 event);
    void pvt_ozwUserAlert(quint8 node, quint32 event, quint8 retry);
    void pvt_manufacturerSpecificDBReady();

    void pvt_nodeModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void pvt_valueModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:

    bool checkHomeId();
    bool checkNodeId(quint8 _node);
    bool checkValueKey(quint64 _vidKey);
    bool convertValueID(quint64 vidKey);


    QTOZWOptions_Internal *m_options;
    OpenZWave::Manager *m_manager;
    QTOZW_Nodes_internal *m_nodeModel;
    QTOZW_ValueIds_internal *m_valueModel;
    QTOZW_Associations_internal *m_associationsModel;
    QTOZW_Log_Internal *m_logModel;
    QVector<quint8> m_validNodes;
    QVector<quint64> m_validValues;
    QMap<quint8, QMap<quint8, bool > > m_associationDefaultsSet;
    QString m_SerialPort;
};


#endif // QTOZWMANAGER_P_H
