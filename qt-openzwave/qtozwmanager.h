#ifndef QTOZWMANAGER_H
#define QTOZWMANAGER_H

#include <QObject>
#include <QUrl>
#include "rep_qtozwmanager_source.h"
#include "rep_qtozwmanager_replica.h"
#include "qtozwnotification.h"
#include "qtozwnodemodel.h"
#include "qtozwvalueidmodel.h"
#include "qtozwassociations.h"

void setupOZW();

enum QTOZW_UserRoles {
    ModelDataChanged = Qt::UserRole
};


namespace OpenZWave {

/* forward Declaration */
class Manager;
class Options;
class Notification;
}


class QTOZWManager_Internal : public QTOZWManagerSimpleSource
{
    Q_OBJECT
public:
    friend class OZWNotification;
    QTOZWManager_Internal(QObject *parent = nullptr);

    QTOZW_Nodes *getNodeModel();
    QTOZW_ValueIds *getValueModel();
    QTOZW_Associations *getAssociationModel();

public Q_SLOTS:
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
    bool sendNodeInfomation(quint8 _node);
    bool replaceFailedNode(quint8 _node);
    bool requestNetworkUpdate(quint8 _node);

    bool checkLatestConfigFileRevision(quint8 const _node);
    bool checkLatestMFSRevision();
    bool downloadLatestConfigFileRevision(quint8 const _node);
    bool downloadLatestMFSRevision();


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
    void pvt_controllerCommand(quint8 command);
    void pvt_ozwNotification(OpenZWave::Notification::NotificationCode event);
    void pvt_ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void pvt_manufacturerSpecificDBReady();

    void pvt_nodeModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void pvt_valueModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:

    bool checkHomeId();
    bool checkNodeId(quint8 _node);
    bool checkValueKey(quint64 _vidKey);
    bool convertValueID(quint64 vidKey);


    OpenZWave::Options *m_options;
    OpenZWave::Manager *m_manager;
    QTOZW_Nodes_internal *m_nodeModel;
    QTOZW_ValueIds_internal *m_valueModel;
    QTOZW_Associations_internal *m_associationsModel;
    QVector<quint8> m_validNodes;
    QVector<quint64> m_validValues;
    QMap<quint8, QMap<quint8, bool > > m_associationDefaultsSet;

};



class QTOZWManager : public QObject {
    Q_OBJECT
public:
    enum connectionType {
        Local,
        Remote,
        Invalid
    };
    Q_ENUM(connectionType)

    QTOZWManager(QObject *parent = nullptr);
    bool initilizeBase();
    bool initilizeSource(bool enableServer);
    bool initilizeReplica(QUrl remoteaddress);

    bool isRunning();

    QAbstractItemModel *getNodeModel();
    QAbstractItemModel *getValueModel();
    QAbstractItemModel *getAssociationModel();

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
    bool sendNodeInfomation(quint8 _node);
    bool replaceFailedNode(quint8 _node);
    bool requestNetworkUpdate(quint8 _node);

    bool checkLatestConfigFileRevision(quint8 const _node);
    bool checkLatestMFSRevision();
    bool downloadLatestConfigFileRevision(quint8 const _node);
    bool downloadLatestMFSRevision();


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
    void controllerCommand(quint8 command);
//    void ozwNotification(OpenZWave::Notification::NotificationCode event);
// void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void manufacturerSpecificDBReady();

    void starting();
    void started(quint32 homeID);
    void stopped(quint32 homeID);
//    void error(QTOZWErrorCodes errorcode);


private Q_SLOTS:
    void onReplicaError(QRemoteObjectNode::ErrorCode);
    void onSourceError(QRemoteObjectHost::ErrorCode);
    void onManagerStateChange(QRemoteObjectReplica::State);
    void onNodeInitialized();
    void onValueInitialized();
    void onAssociationInitialized();
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
    QRemoteObjectReplica::State m_managerState;
    bool m_nodeState;
    bool m_valuesState;
    bool m_associationsState;

    QAbstractItemModel *m_nodeModel;
    QAbstractItemModel *m_valueModel;
    QAbstractItemModel *m_associationModel;

    bool m_running;
};



#endif // QTOZWMANAGER_H
