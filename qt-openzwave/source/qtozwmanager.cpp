//-----------------------------------------------------------------------------
//
//	qtozwmanager.cpp
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
#ifndef WIN32
#include <unistd.h>
#endif
#include <QDebug>
#include <QAbstractItemModel>
#include <QAbstractItemModelReplica>
#include "qt-openzwave/qtozwmanager.h"
#include "qtozwmanager_p.h"
#include "qtozw_logging.h"


QTOZWManager::QTOZWManager(QObject *parent)
    : QObject(parent),
    m_running(false),
    m_ozwdatabasepath(""),
    m_ozwuserpath("")
{

}

bool QTOZWManager::initilizeBase() {
    return true;
}
bool QTOZWManager::initilizeSource(bool enableServer) {
    initilizeBase();
    this->m_connectionType = connectionType::Local;

    /* initilize the OZW Logging Class - To Pass Log Messages to us */

    QTOZW_Log_Internal *t_logModel = new QTOZW_Log_Internal(this);
    OpenZWave::Log::SetLoggingClass(static_cast<OpenZWave::i_LogImpl*>(t_logModel));

    /* Initilize the OZW Options Static Class */
    if (!this->m_ozwdatabasepath.exists()) {
        qCWarning(manager) << "Database Path Does Not Exist: " << this->m_ozwdatabasepath;
        return false;
    }
    if (!this->m_ozwuserpath.exists()) {
        qCWarning(manager) << "User Path Does Not Exist: " << this->m_ozwuserpath;
        return false;
    }
    qCDebug(manager) << "Database Path: " << this->m_ozwdatabasepath.path().append("/") << " User Path" << this->m_ozwuserpath.path().append("/");
    QString dbPath = this->m_ozwdatabasepath.path();
    QString userPath = this->m_ozwuserpath.path();
    /* OZW expects the paths to end with a / otherwise it treats it as a file */
    if (dbPath.at(dbPath.size()-1) != "/")
        dbPath.append("/");
    if (userPath.at(userPath.size()-1) != "/")
        userPath.append("/");
    OpenZWave::Options::Create(dbPath.toStdString(), userPath.toStdString(), "");


    /* Initilize our QTOZWManager Class */

    this->d_ptr_internal = new QTOZWManager_Internal(this);
    this->d_ptr_internal->setLogModel(t_logModel);
    this->m_ozwoptions = new QTOZWOptions(QTOZWOptions::connectionType::Local, this);
    if (enableServer) {
        this->m_sourceNode = new QRemoteObjectHost(QUrl(QStringLiteral("tcp://0.0.0.0:1983")), this);
        QObject::connect(this->m_sourceNode, &QRemoteObjectHost::error, this, &QTOZWManager::onSourceError);
        //this->m_sourceNode->setHeartbeatInterval(1000);
        this->m_sourceNode->enableRemoting<QTOZWManagerSourceAPI>(this->d_ptr_internal);
        this->m_ozwoptions->initilizeSource(this->m_sourceNode);
        QVector<int> roles;
        roles << Qt::DisplayRole << Qt::EditRole << Qt::ToolTipRole;
        this->m_sourceNode->enableRemoting(this->d_ptr_internal->getNodeModel(), "QTOZW_nodeModel", roles);
        this->m_sourceNode->enableRemoting(this->d_ptr_internal->getValueModel(), "QTOZW_valueModel", roles);
        this->m_sourceNode->enableRemoting(this->d_ptr_internal->getAssociationModel(), "QTOZW_associationModel", roles);
        this->m_sourceNode->enableRemoting(this->d_ptr_internal->getLogModel(), "QTOZW_Log");
    }
    connectSignals();
    emit this->ready();
    return true;
}

bool QTOZWManager::initilizeReplica(QUrl remote) {
    initilizeBase();
    this->m_connectionType = connectionType::Remote;
    this->m_replicaNode = new QRemoteObjectNode(this);
    this->m_ozwoptions = new QTOZWOptions(QTOZWOptions::connectionType::Remote, this);
    QObject::connect(this->m_replicaNode, &QRemoteObjectNode::error, this, &QTOZWManager::onReplicaError);
    if (this->m_replicaNode->connectToNode(remote)) {
        this->d_ptr_replica = this->m_replicaNode->acquire<QTOZWManagerReplica>("QTOZWManager");
        QObject::connect(this->d_ptr_replica, &QTOZWManagerReplica::stateChanged, this, &QTOZWManager::onManagerStateChange);
        this->m_ozwoptions->initilizeReplica(this->m_replicaNode);
        QObject::connect(this->m_ozwoptions, &QTOZWOptions::onOptionStateChange, this, &QTOZWManager::onOptionsStateChange);
        this->m_nodeModel = this->m_replicaNode->acquireModel("QTOZW_nodeModel", QtRemoteObjects::InitialAction::PrefetchData);
        QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_nodeModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onNodeInitialized);
        this->m_valueModel= this->m_replicaNode->acquireModel("QTOZW_valueModel", QtRemoteObjects::InitialAction::PrefetchData);
        QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_valueModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onValueInitialized);
        this->m_associationModel= this->m_replicaNode->acquireModel("QTOZW_associationModel", QtRemoteObjects::InitialAction::PrefetchData);
        QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_associationModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onAssociationInitialized);
        this->m_logModel = this->m_replicaNode->acquireModel("QTOZW_logModel", QtRemoteObjects::InitialAction::FetchRootSize);
        QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_logModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onLogInitialized);
    }
    return true;
}

void QTOZWManager::onReplicaError(QRemoteObjectNode::ErrorCode error) {
    qCWarning(manager) << "Replica Error: " << error;
    /* raise this upto the application */
}

void QTOZWManager::onSourceError(QRemoteObjectHost::ErrorCode error) {
    qCWarning(manager) << "Host Error: " << error;
    /* raise this upto the application */
}

void QTOZWManager::onManagerStateChange(QRemoteObjectReplica::State state) {
    this->m_managerState = state;
    this->checkReplicaReady();
}

void QTOZWManager::onOptionsStateChange(QRemoteObjectReplica::State state) {
    this->m_optionsState = state;
    this->checkReplicaReady();
}

void QTOZWManager::onNodeInitialized() {
    this->m_nodeState = true;
    this->checkReplicaReady();
}
void QTOZWManager::onValueInitialized() {
    this->m_valuesState = true;
    this->checkReplicaReady();
}
void QTOZWManager::onAssociationInitialized() {
    this->m_associationsState = true;
    this->checkReplicaReady();
}

void QTOZWManager::onLogInitialized() {
    this->m_logState = true;
    this->checkReplicaReady();
}

void QTOZWManager::checkReplicaReady() {
    if ((this->m_managerState == QRemoteObjectReplica::State::Valid) &&
            (this->m_optionsState == QRemoteObjectReplica::State::Valid) &&
                (this->m_nodeState == true) &&
                    (this->m_valuesState == true) &&
                        (this->m_associationsState == true) &&
                            (this->m_logState == true)) {
        /* have to connect all the d_ptr SIGNALS to our SIGNALS now */
        connectSignals();
        emit this->ready();
    }
}

bool QTOZWManager::isRunning() {
    return this->m_running;
}

void QTOZWManager::setStarted() {
    qCDebug(manager) << "setStarted";
    this->m_running = true;
}

void QTOZWManager::setStopped() {
    this->m_running = false;
}

QAbstractItemModel *QTOZWManager::getNodeModel() {
    if (this->m_connectionType == connectionType::Local) {
        return this->d_ptr_internal->getNodeModel();
    } else {
        return this->m_nodeModel;
    }
}
QAbstractItemModel *QTOZWManager::getValueModel() {
    if (this->m_connectionType == connectionType::Local) {
        return this->d_ptr_internal->getValueModel();
    } else {
        return this->m_valueModel;
    }

}
QAbstractItemModel *QTOZWManager::getAssociationModel() {
    if (this->m_connectionType == connectionType::Local) {
        return this->d_ptr_internal->getAssociationModel();
    } else {
        return this->m_associationModel;
    }
}

QTOZWOptions *QTOZWManager::getOptions() {
    return this->m_ozwoptions;
}

QAbstractItemModel *QTOZWManager::getLogModel() {
    if (this->m_connectionType == connectionType::Local) {
        return this->d_ptr_internal->getLogModel();
    } else {
        return this->m_logModel;
    }
}


#define CONNECT_DPTR(x)     if (this->m_connectionType == connectionType::Local) { \
        QObject::connect(this->d_ptr_internal, &QTOZWManager_Internal::x, this, &QTOZWManager::x);\
    } else { \
        QObject::connect(this->d_ptr_replica, &QTOZWManagerReplica::x, this, &QTOZWManager::x); \
    };

#define CONNECT_DPTR1(x, y)     if (this->m_connectionType == connectionType::Local) { \
        QObject::connect(this->d_ptr_internal, &QTOZWManager_Internal::x, this, &QTOZWManager::y);\
    } else { \
        QObject::connect(this->d_ptr_replica, &QTOZWManagerReplica::x, this, &QTOZWManager::y); \
    };


void QTOZWManager::connectSignals() {
    CONNECT_DPTR(valueAdded);
    CONNECT_DPTR(valueRemoved);
    CONNECT_DPTR(valueChanged);
    CONNECT_DPTR(valueRefreshed);
    CONNECT_DPTR(nodeNew);
    CONNECT_DPTR(nodeAdded);
    CONNECT_DPTR(nodeRemoved);
    CONNECT_DPTR(nodeReset);
    CONNECT_DPTR(nodeNaming);
    CONNECT_DPTR(nodeEvent);
    CONNECT_DPTR(nodeProtocolInfo);
    CONNECT_DPTR(nodeEssentialNodeQueriesComplete);
    CONNECT_DPTR(nodeQueriesComplete);
    CONNECT_DPTR(driverReady);
    CONNECT_DPTR(driverFailed);
    CONNECT_DPTR(driverReset);
    CONNECT_DPTR(driverRemoved);
    CONNECT_DPTR(driverAllNodesQueriedSomeDead);
    CONNECT_DPTR(driverAllNodesQueried);
    CONNECT_DPTR(driverAwakeNodesQueried);
    CONNECT_DPTR(controllerCommand);
    CONNECT_DPTR(ozwNotification);
    CONNECT_DPTR(ozwUserAlert);
    CONNECT_DPTR(manufacturerSpecificDBReady);
    CONNECT_DPTR(starting);
    CONNECT_DPTR(started);
    CONNECT_DPTR(stopped);
//        CONNECT_DPTR(error);

    /* some extra internal Signals we need to track */
    CONNECT_DPTR1(started, setStarted);
    CONNECT_DPTR1(stopped, setStopped)

}

#define CALL_DPTR(x) if (this->m_connectionType == QTOZWManager::connectionType::Local) this->d_ptr_internal->x; else this->d_ptr_replica->x;
#define CALL_DPTR_RTN(x, y) if (this->m_connectionType == QTOZWManager::connectionType::Local) \
    return this->d_ptr_internal->x; \
    else { \
    QRemoteObjectPendingReply<y> res = this->d_ptr_replica->x; \
    res.waitForFinished(); \
    return res.returnValue(); \
    }


bool QTOZWManager::open(QString serialPort) {
    CALL_DPTR_RTN(open(serialPort), bool);
}
bool QTOZWManager::refreshNodeInfo(quint8 _node) {
    CALL_DPTR_RTN(refreshNodeInfo(_node), bool);
}
bool QTOZWManager::requestNodeState(quint8 _node) {
    CALL_DPTR_RTN(requestNodeState(_node), bool);
}
bool QTOZWManager::requestNodeDynamic(quint8 _node) {
    CALL_DPTR_RTN(requestNodeDynamic(_node), bool);
}
bool QTOZWManager::setConfigParam(quint8 _node, quint8 _param, qint32 _value, quint8 const _size) {
    CALL_DPTR_RTN(setConfigParam(_node, _param, _value, _size), bool);
}
void QTOZWManager::requestConfigParam(quint8 _node, quint8 _param) {
    CALL_DPTR(requestConfigParam(_node, _param));
}
void QTOZWManager::requestAllConfigParam(quint8 _node) {
    CALL_DPTR(requestAllConfigParam(_node));
}
void QTOZWManager::softResetController() {
    CALL_DPTR(softResetController());
}
void QTOZWManager::hardResetController() {
    CALL_DPTR(hardResetController());
}
bool QTOZWManager::cancelControllerCommand() {
    CALL_DPTR_RTN(cancelControllerCommand(), bool);
}
void QTOZWManager::testNetworkNode(quint8 _node, quint32 const _count) {
    CALL_DPTR(testNetworkNode(_node, _count));
}
void QTOZWManager::testNetwork(quint32 const _count) {
    CALL_DPTR(testNetwork(_count));
}
void QTOZWManager::healNetworkNode(quint8 _node, bool _doRR) {
    CALL_DPTR(healNetworkNode(_node, _doRR));
}
void QTOZWManager::healNetwork(bool _doRR) {
    CALL_DPTR(healNetwork(_doRR));
}
bool QTOZWManager::addNode(bool _doSecure) {
    CALL_DPTR_RTN(addNode(_doSecure), bool);
}
bool QTOZWManager::removeNode() {
    CALL_DPTR_RTN(removeNode(), bool);
}
bool QTOZWManager::removeFailedNode(quint8 _node) {
    CALL_DPTR_RTN(removeFailedNode(_node), bool);
}
bool QTOZWManager::hasNodeFailed(quint8 _node) {
    CALL_DPTR_RTN(hasNodeFailed(_node), bool);
}
bool QTOZWManager::requestNodeNeighborUpdate(quint8 _node) {
    CALL_DPTR_RTN(requestNodeNeighborUpdate(_node), bool);
}
bool QTOZWManager::assignReturnRoute(quint8 _node) {
    CALL_DPTR_RTN(assignReturnRoute(_node), bool);
}
bool QTOZWManager::deleteAllReturnRoute(quint8 _node) {
    CALL_DPTR_RTN(deleteAllReturnRoute(_node), bool);
}
bool QTOZWManager::sendNodeInformation(quint8 _node) {
    CALL_DPTR_RTN(sendNodeInformation(_node), bool);
}
bool QTOZWManager::replaceFailedNode(quint8 _node) {
    CALL_DPTR_RTN(replaceFailedNode(_node), bool)
}
bool QTOZWManager::requestNetworkUpdate(quint8 _node) {
    CALL_DPTR_RTN(requestNetworkUpdate(_node), bool);
}
QString QTOZWManager::GetMetaData(quint8 _node, QTOZWManagerSource::QTOZWMetaDataField _field) {
    if (this->m_connectionType == QTOZWManager::connectionType::Local) {
        return this->d_ptr_internal->GetMetaData(_node, _field); \
    } else {
        QRemoteObjectPendingReply<QString> res = this->d_ptr_replica->GetMetaData(_node, static_cast<QTOZWManagerReplica::QTOZWMetaDataField>(_field));
        res.waitForFinished();
        return res.returnValue();
    }
}

QByteArray QTOZWManager::GetMetaDataProductPic(quint8 _node) {
    CALL_DPTR_RTN(GetMetaDataProductPic(_node), QByteArray);
}

QString QTOZWManager::GetNodeQueryStage(const quint8 _node) {
    CALL_DPTR_RTN(GetNodeQueryStage(_node), QString);
}
NodeStatistics QTOZWManager::GetNodeStatistics(const quint8 _node) {
    CALL_DPTR_RTN(GetNodeStatistics(_node), NodeStatistics);
}

DriverStatistics QTOZWManager::GetDriverStatistics() {
    CALL_DPTR_RTN(GetDriverStatistics(), DriverStatistics);
}

QVector<quint8> QTOZWManager::GetNodeNeighbors(quint8 const _node) {
    CALL_DPTR_RTN(GetNodeNeighbors(_node), QVector<quint8>);
}

bool QTOZWManager::IsNodeFailed(const quint8 _node) {
    CALL_DPTR_RTN(IsNodeFailed(_node), bool);
}
bool QTOZWManager::checkLatestConfigFileRevision(quint8 const _node) {
    CALL_DPTR_RTN(checkLatestConfigFileRevision(_node), bool);
}
bool QTOZWManager::checkLatestMFSRevision() {
    CALL_DPTR_RTN(checkLatestMFSRevision(), bool);
}
bool QTOZWManager::downloadLatestConfigFileRevision(quint8 const _node) {
    CALL_DPTR_RTN(downloadLatestConfigFileRevision(_node), bool);
}
bool QTOZWManager::downloadLatestMFSRevision() {
    CALL_DPTR_RTN(downloadLatestMFSRevision(), bool);
}

QString QTOZWManager::getCommandClassString(quint8 const _cc) {
    CALL_DPTR_RTN(getCommandClassString(_cc), QString);
}



void QTOZWManager::setOZWDatabasePath(QDir path) {
    if (path.exists())
        this->m_ozwdatabasepath = path;
    else
        qCWarning(manager) << "Database Path " << path << " doesn't exist";

}
void QTOZWManager::setOZWUserPath(QDir path) {
    if (path.exists())
        this->m_ozwuserpath = path;
    else
        qCWarning(manager) << "User Path " << path << " doesn't exist";
}

