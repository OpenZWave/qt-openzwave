#ifndef QTOZWMANAGER_P_H
#define QTOZWMANAGER_P_H

#include <QObject>
#include <QUrl>
#include "qtozw_logging.h"
#include "rep_qtozwmanager_source.h"
#include "rep_qtozwmanager_replica.h"
#include "qtozwnotification.h"
#include "qt-openzwave/qtozwmanager.h"
#include "qtozwnodemodel_p.h"
#include "qtozwvalueidmodel_p.h"
#include "qtozwassociationmodel_p.h"

/* OZW Includes */
#include "Manager.h"
#include "Options.h"
#include "Notification.h"
#include "platform/Log.h"
#include "OZWException.h"


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
    QString GetMetaData(quint8 _node, QTOZWManagerSource::QTOZWMetaDataField _field);
    QByteArray GetMetaDataProductPic(quint8 _node);

    QString GetNodeQueryStage(quint8 const node);
    NodeStatistics GetNodeStatistics(quint8 const node);

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


#endif // QTOZWMANAGER_P_H
