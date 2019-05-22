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
    bool refreshNodeInfo(uint8_t _node);
    bool requestNodeState(uint8_t _node);
    bool requestNodeDynamic(uint8_t _node);

    bool setConfigParam(uint8_t _node, uint8_t _param, int32_t _value, uint8_t const _size);
    void requestConfigParam(uint8_t _node, uint8_t _param);
    void requestAllConfigParam(uint8_t _node);

    void softResetController();
    void hardResetController();

    bool cancelControllerCommand();

    void testNetworkNode(uint8_t _node, uint32_t const _count);
    void testNetwork(uint32_t const _count);
    void healNetworkNode(uint8_t _node, bool _doRR);
    void healNetwork(bool _doRR);
    bool addNode(bool _doSecure);
    bool removeNode();
    bool removeFailedNode(uint8_t _node);
    bool hasNodeFailed(uint8_t _node);
    bool requestNodeNeighborUpdate(uint8_t _node);
    bool assignReturnRoute(uint8_t _node);
    bool deleteAllReturnRoute(uint8_t _node);
    bool sendNodeInfomation(uint8_t _node);
    bool replaceFailedNode(uint8_t _node);
    bool requestNetworkUpdate(uint8_t _node);

    bool checkLatestConfigFileRevision(uint8_t const _node);
    bool checkLatestMFSRevision();
    bool downloadLatestConfigFileRevision(uint8_t const _node);
    bool downloadLatestMFSRevision();


    /* these slots are called from our OZWNotification Class. Applications should not call them */
    void pvt_valueAdded(uint64_t vidKey);
    void pvt_valueRemoved(uint64_t vidKey);
    void pvt_valueChanged(uint64_t vidKey);
    void pvt_valueRefreshed(uint64_t vidKey);
    void pvt_valuePollingEnabled(uint64_t vidKey);
    void pvt_valuePollingDisabled(uint64_t vidKey);
    void pvt_nodeGroupChanged(uint8_t node, uint8_t group);
    void pvt_nodeNew(uint8_t node);
    void pvt_nodeAdded(uint8_t node);
    void pvt_nodeRemoved(uint8_t node);
    void pvt_nodeReset(uint8_t node);
    void pvt_nodeNaming(uint8_t node);
    void pvt_nodeEvent(uint8_t node, uint8_t event);
    void pvt_nodeProtocolInfo(uint8_t node);
    void pvt_nodeEssentialNodeQueriesComplete(uint8_t node);
    void pvt_nodeQueriesComplete(uint8_t node);
    void pvt_driverReady(uint32_t homeID);
    void pvt_driverFailed(uint32_t homeID);
    void pvt_driverReset(uint32_t homeID);
    void pvt_driverRemoved(uint32_t homeID);
    void pvt_driverAllNodesQueriedSomeDead();
    void pvt_driverAllNodesQueried();
    void pvt_driverAwakeNodesQueried();
    void pvt_controllerCommand(uint8_t command);
    void pvt_ozwNotification(OpenZWave::Notification::NotificationCode event);
    void pvt_ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void pvt_manufacturerSpecificDBReady();

    void pvt_nodeModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void pvt_valueModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    bool Lock();
    bool Unlock();

private:

    bool checkHomeId();
    bool checkNodeId(uint8_t _node);
    bool checkValueKey(uint64_t _vidKey);
    bool convertValueID(uint64_t vidKey);


    OpenZWave::Options *m_options;
    OpenZWave::Manager *m_manager;
    QTOZW_Nodes_internal *m_nodeModel;
    QTOZW_ValueIds_internal *m_valueModel;
    QTOZW_Associations_internal *m_associationsModel;
    pthread_mutex_t m_manager_mutex;
    QVector<uint8_t> m_validNodes;
    QVector<uint64_t> m_validValues;
    QMap<uint8_t, QMap<uint8_t, bool > > m_associationDefaultsSet;

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
    bool refreshNodeInfo(uint8_t _node);
    bool requestNodeState(uint8_t _node);
    bool requestNodeDynamic(uint8_t _node);

    bool setConfigParam(uint8_t _node, uint8_t _param, int32_t _value, uint8_t const _size);
    void requestConfigParam(uint8_t _node, uint8_t _param);
    void requestAllConfigParam(uint8_t _node);

    void softResetController();
    void hardResetController();

    bool cancelControllerCommand();

    void testNetworkNode(uint8_t _node, uint32_t const _count);
    void testNetwork(uint32_t const _count);
    void healNetworkNode(uint8_t _node, bool _doRR);
    void healNetwork(bool _doRR);
    bool addNode(bool _doSecure);
    bool removeNode();
    bool removeFailedNode(uint8_t _node);
    bool hasNodeFailed(uint8_t _node);
    bool requestNodeNeighborUpdate(uint8_t _node);
    bool assignReturnRoute(uint8_t _node);
    bool deleteAllReturnRoute(uint8_t _node);
    bool sendNodeInfomation(uint8_t _node);
    bool replaceFailedNode(uint8_t _node);
    bool requestNetworkUpdate(uint8_t _node);

    bool checkLatestConfigFileRevision(uint8_t const _node);
    bool checkLatestMFSRevision();
    bool downloadLatestConfigFileRevision(uint8_t const _node);
    bool downloadLatestMFSRevision();


Q_SIGNALS:
    void ready();
    void valueAdded(uint64_t vidKey);
    void valueRemoved(uint64_t vidKey);
    void valueChanged(uint64_t vidKey);
    void valueRefreshed(uint64_t vidKey);
    void nodeNew(uint8_t node);
    void nodeAdded(uint8_t node);
    void nodeRemoved(uint8_t node);
    void nodeReset(uint8_t node);
    void nodeNaming(uint8_t node);
    void nodeEvent(uint8_t node, uint8_t event);
    void nodeProtocolInfo(uint8_t node);
    void nodeEssentialNodeQueriesComplete(uint8_t node);
    void nodeQueriesComplete(uint8_t node);
    void driverReady(uint32_t homeID);
    void driverFailed(uint32_t homeID);
    void driverReset(uint32_t homeID);
    void driverRemoved(uint32_t homeID);
    void driverAllNodesQueriedSomeDead();
    void driverAllNodesQueried();
    void driverAwakeNodesQueried();
    void controllerCommand(uint8_t command);
//    void ozwNotification(OpenZWave::Notification::NotificationCode event);
// void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void manufacturerSpecificDBReady();

    void starting();
    void started(uint32_t homeID);
    void stopped(uint32_t homeID);
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
