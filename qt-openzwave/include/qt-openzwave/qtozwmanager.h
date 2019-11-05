//-----------------------------------------------------------------------------
//
//	qtozwmanager.h
//
//	Main Manager Class for QT-OpenZWave
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

#ifndef QTOZWMANAGER_H
#define QTOZWMANAGER_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QUrl>
#include <QtRemoteObjects>

#include "qt-openzwave/rep_qtozwmanager_source.h"
#include "qt-openzwave/qtozwoptions.h"
#include "qt-openzwave/qtozwnodemodel.h"
#include "qt-openzwave/qtozwvalueidmodel.h"
#include "qt-openzwave/qtozwassociationmodel.h"

enum QTOZW_UserRoles {
    ModelDataChanged = Qt::UserRole
};

class QTOZWManager_Internal;
class QTOZWManagerReplica;
class QTOZWOptionsReplica;

class QTOPENZWAVESHARED_EXPORT QTOZWManager : public QObject {
    Q_OBJECT
public:
    enum connectionType {
        Local,
        Remote,
        Invalid
    };
    Q_ENUM(connectionType)
    Q_PROPERTY(QDir OZWDatabasePath READ OZWDatabasePath WRITE setOZWDatabasePath)
    Q_PROPERTY(QDir OZWUserPath READ OZWUserPath WRITE setOZWUserPath)


    QTOZWManager(QObject *parent = nullptr);
    bool initilizeBase();
    bool initilizeSource(bool enableServer);
    bool initilizeReplica(QUrl remoteaddress);

    bool isRunning();

    QAbstractItemModel *getNodeModel();
    QAbstractItemModel *getValueModel();
    QAbstractItemModel *getAssociationModel();
    QAbstractItemModel *getLogModel();
    QTOZWOptions *getOptions();

    /* OpenZWave::Manager methods */
    bool open(QString serialPort);
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

    QString GetNodeQueryStage(quint8 const _node);
    NodeStatistics GetNodeStatistics(quint8 const node);
    DriverStatistics GetDriverStatistics(); 
    QVector<quint8> GetNodeNeighbors(quint8 const node);
    bool IsNodeFailed(const quint8 _node);

    bool checkLatestConfigFileRevision(quint8 const _node);
    bool checkLatestMFSRevision();
    bool downloadLatestConfigFileRevision(quint8 const _node);
    bool downloadLatestMFSRevision();

    QString getCommandClassString(quint8 const _cc);

/* Property Methods */
    QDir OZWDatabasePath() { return this->m_ozwdatabasepath; }
    QDir OZWUserPath() { return this->m_ozwuserpath; }
    void setOZWDatabasePath(QDir path);
    void setOZWUserPath(QDir path);


Q_SIGNALS:
    void ready();
    void valueAdded(quint64 vidKey);
    void valueRemoved(quint64 vidKey);
    void valueChanged(quint64 vidKey);
    void valueRefreshed(quint64 vidKey);
    void nodeNew(quint8 node);
    void nodeAdded(quint8 node);
    void nodeRemoved(quint8 node);
    void nodeReset(quint8 node);
    void nodeNaming(quint8 node);
    void nodeEvent(quint8 node, quint8 event);
    void nodeProtocolInfo(quint8 node);
    void nodeEssentialNodeQueriesComplete(quint8 node);
    void nodeQueriesComplete(quint8 node);
    void driverReady(quint32 homeID);
    void driverFailed(quint32 homeID);
    void driverReset(quint32 homeID);
    void driverRemoved(quint32 homeID);
    void driverAllNodesQueriedSomeDead();
    void driverAllNodesQueried();
    void driverAwakeNodesQueried();
    void controllerCommand(quint8 node, NotificationTypes::QTOZW_Notification_Controller_Cmd command, NotificationTypes::QTOZW_Notification_Controller_State state, NotificationTypes::QTOZW_Notification_Controller_Error error);
    void ozwNotification(quint8 node, NotificationTypes::QTOZW_Notification_Code event);
    void ozwUserAlert(quint8 node, NotificationTypes::QTOZW_Notification_User event, quint8 retry);
    void manufacturerSpecificDBReady();

    void starting();
    void started(quint32 homeID);
    void stopped(quint32 homeID);
//    void error(QTOZWErrorCodes errorcode);



private Q_SLOTS:
    void onReplicaError(QRemoteObjectNode::ErrorCode);
    void onSourceError(QRemoteObjectHost::ErrorCode);
    void onManagerStateChange(QRemoteObjectReplica::State);
    void onOptionsStateChange(QRemoteObjectReplica::State);
    void onNodeInitialized();
    void onValueInitialized();
    void onAssociationInitialized();
    void onLogInitialized();
    void setStarted();
    void setStopped();



private:

    void checkReplicaReady();
    void connectSignals();
    connectionType m_connectionType;
    QRemoteObjectNode *m_replicaNode;
    QRemoteObjectHost *m_sourceNode;
    QTOZWManager_Internal *d_ptr_internal;
    QTOZWManagerReplica *d_ptr_replica;
    QTOZWOptions *m_ozwoptions;
    QRemoteObjectReplica::State m_managerState;
    QRemoteObjectReplica::State m_optionsState;
    bool m_nodeState;
    bool m_valuesState;
    bool m_associationsState;
    bool m_logState;

    QAbstractItemModel *m_nodeModel;
    QAbstractItemModel *m_valueModel;
    QAbstractItemModel *m_associationModel;
    QAbstractItemModel *m_logModel;

    bool m_running;
    QDir m_ozwdatabasepath;
    QDir m_ozwuserpath;
};



#endif // QTOZWMANAGER_H
