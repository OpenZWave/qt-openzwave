#ifndef QTOZWMANAGER_H
#define QTOZWMANAGER_H

#include <QObject>
#include <QUrl>
#include <QtRemoteObjects>

#include "qtozwnodemodel.h"
#include "qtozwvalueidmodel.h"
#include "qtozwassociationmodel.h"

enum QTOZW_UserRoles {
    ModelDataChanged = Qt::UserRole
};

class QTOZWManager_Internal;
class QTOZWManagerReplica;


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
