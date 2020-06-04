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
#include <QWebSocket>
#include "qt-openzwave/qtozwmanager.h"
#include "qt-openzwave/websocketiodevice.h"
#include "qt-openzwave/qtozwreplica.h"
#include "qtozwmanager_p.h"
#include "qtozw_logging.h"

#define REP_INTERNAL_CLASS QTOZWManager_Internal
#define REP_REPLICA_CLASS QTOZWManagerReplica
#define REP_PUBLIC_CLASS QTOZWManager


QTOZWManager::QTOZWManager(QObject *parent)
    : QTOZWReplicaBase(ConnectionType::Type::Invalid, parent),
    m_running(false),
    m_ozwconfigpath(""),
    m_ozwuserpath(""),
    m_clientAuth("")
{

}

bool QTOZWManager::initilizeBase() {
    return true;
}
#ifndef Q_OS_WASM
bool QTOZWManager::initilizeSource(bool enableServer) {
    initilizeBase();
    this->setConnectionType(ConnectionType::Type::Local);

    /* Initilize the OZW Options Static Class */
    if (!QFileInfo::exists(this->m_ozwconfigpath)) {
        qCWarning(manager) << "Config Path Does Not Exist: " << this->m_ozwconfigpath;
        return false;
    }
    if (!QFileInfo::exists(this->m_ozwuserpath)) {
        qCWarning(manager) << "User Path Does Not Exist: " << this->m_ozwuserpath;
        return false;
    }

    qCDebug(manager) << "Database Path: " << QFileInfo(this->m_ozwconfigpath.append("/")).absoluteFilePath() << " User Path" << QFileInfo(this->m_ozwuserpath.append("/")).absoluteFilePath();

    OpenZWave::Options::Create(QFileInfo(this->m_ozwconfigpath.append("/")).absoluteFilePath().toStdString(), QFileInfo(this->m_ozwuserpath.append("/")).absoluteFilePath().toStdString(), "");

    /* Initilize our QTOZWManager Class */

    this->m_log = new QTOZWLog(ConnectionType::Type::Local, this);
    connect(this, &QTOZWManager::readyChanged, this->m_log, &QTOZWLog::setReady);

    this->m_ozwoptions = new QTOZWOptions(ConnectionType::Type::Local, this);
    connect(this, &QTOZWManager::readyChanged, this->m_ozwoptions, &QTOZWOptions::setReady);
    
    this->d_ptr_internal = new QTOZWManager_Internal(this);

    if (enableServer) {
        this->m_webSockServer = new QWebSocketServer(QStringLiteral("WS QtRO"), QWebSocketServer::NonSecureMode, this);
        QObject::connect(this->m_webSockServer, &QWebSocketServer::newConnection, this, &QTOZWManager::newConnection);
        QObject::connect(this->m_webSockServer, &QWebSocketServer::acceptError, this, &QTOZWManager::acceptError);
        QObject::connect(this->m_webSockServer, &QWebSocketServer::serverError, this, &QTOZWManager::serverError);
        QObject::connect(this->m_webSockServer, &QWebSocketServer::peerVerifyError, this, &QTOZWManager::peerVerifyError);
        QObject::connect(this->m_webSockServer, &QWebSocketServer::sslErrors, this, &QTOZWManager::sslErrors);
        if (!this->m_webSockServer->listen(QHostAddress::Any, 1983)) {
            qCWarning(manager) << "Couldn't Start WebSocket Server Listening On Socket" << this->m_webSockServer->errorString();
            /* we continue on, just in case */
        } else {
            this->m_sourceNode = new QRemoteObjectHost(this);
            this->m_sourceNode->setHostUrl(this->m_webSockServer->serverAddress().toString(), QRemoteObjectHost::AllowExternalRegistration);


            QObject::connect(this->m_sourceNode, &QRemoteObjectHost::error, this, &QTOZWManager::onSourceError);
            //this->m_sourceNode->setHeartbeatInterval(1000);
            this->m_sourceNode->enableRemoting<QTOZWManagerSourceAPI>(this->d_ptr_internal);

            this->m_log->initilizeSource(this->m_sourceNode);

            this->m_ozwoptions->initilizeSource(this->m_sourceNode);
            
            QVector<int> roles;
            roles << Qt::DisplayRole << Qt::EditRole << Qt::ToolTipRole;
            this->m_sourceNode->enableRemoting(this->d_ptr_internal->getNodeModel(), "QTOZW_nodeModel", roles);
            this->m_sourceNode->enableRemoting(this->d_ptr_internal->getValueModel(), "QTOZW_valueModel", roles);
            this->m_sourceNode->enableRemoting(this->d_ptr_internal->getAssociationModel(), "QTOZW_associationModel", roles);

            this->m_sourceNode->enableRemoting<QTOZWLogSourceAPI>(this->d_ptr_internal->getLog());
        }
    } 
    connectSignals();
    setReady(true);
    return true;
}

void QTOZWManager::newConnection() {
    while (QWebSocket *conn = this->m_webSockServer->nextPendingConnection()) {
#ifdef WS_USE_SSL
        // Always use secure connections when available
        QSslConfiguration sslConf;
        QFile certFile(QStringLiteral(":/sslcert/server.crt"));
        if (!certFile.open(QIODevice::ReadOnly))
            qFatal("Can't open client.crt file");
        sslConf.setLocalCertificate(QSslCertificate{certFile.readAll()});

        QFile keyFile(QStringLiteral(":/sslcert/server.key"));
        if (!keyFile.open(QIODevice::ReadOnly))
            qFatal("Can't open client.key file");
        sslConf.setPrivateKey(QSslKey{keyFile.readAll(), QSsl::Rsa});

        sslConf.setPeerVerifyMode(QSslSocket::VerifyPeer);
        conn->setSslConfiguration(sslConf);
        QObject::connect(conn, &QWebSocket::sslErrors, conn, &QWebSocket::deleteLater);
#endif
        qCInfo(manager) << "New Client WebSocket Connection" << conn->peerAddress();
        QObject::connect(conn, &QWebSocket::disconnected, conn, &QWebSocket::deleteLater);
        QObject::connect(conn,  QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), conn, &QWebSocket::deleteLater);
        WebSocketIoDevice *ioDevice = new WebSocketIoDevice(conn, false, m_clientAuth, this);
        QObject::connect(conn, &QWebSocket::destroyed, ioDevice, &WebSocketIoDevice::deleteLater);
        QObject::connect(ioDevice, &WebSocketIoDevice::authenticated, this, &QTOZWManager::serverAuthenticated);
        QObject::connect(ioDevice, &WebSocketIoDevice::authError, this, &QTOZWManager::serverAuthError);
        QObject::connect(ioDevice, &WebSocketIoDevice::disconnected, this, &QTOZWManager::serverDisconnected);
    }
}

void QTOZWManager::serverDisconnected() {
    WebSocketIoDevice *ioDevice = qobject_cast<WebSocketIoDevice*>(QObject::sender());
    if (ioDevice) {
        qCInfo(manager) << "Client Connection Disconnected!";
    } else {
        qCWarning(manager) << "Missing Sender Object in serverDisconnected";
    }

}

void QTOZWManager::serverAuthenticated() {
    WebSocketIoDevice *ioDevice = qobject_cast<WebSocketIoDevice*>(QObject::sender());
    if (ioDevice) {
        qCInfo(manager) << "Client Connection Authenticated!";
        this->m_sourceNode->addHostSideConnection(ioDevice);
    } else {
        qCWarning(manager) << "Missing Sender Object in serverAuthenticated";
    }
}

void QTOZWManager::serverAuthError(QString error) {
    WebSocketIoDevice *ioDevice = qobject_cast<WebSocketIoDevice*>(QObject::sender());
    if (ioDevice) {
        qCInfo(manager) << "Client Connection Authentication Failure" << error;
    } else {
        qCWarning(manager) << "Missing Sender Object in serverAuthError";
    }
}

void QTOZWManager::acceptError(QAbstractSocket::SocketError socketError) {
    qCWarning(manager) << "Accept Error on WebSocket Server Port" << socketError << this->m_webSockServer->errorString();
}
void QTOZWManager::serverError(QWebSocketProtocol::CloseCode closeCode) {
    qCWarning(manager) << "Server Error on WebSocket Server Port" << closeCode << this->m_webSockServer->errorString();
}
void QTOZWManager::peerVerifyError(const QSslError &error) {
    qCWarning(manager) << "peer Verify Error on WebSocket Server Port" << error << this->m_webSockServer->errorString();
}
void QTOZWManager::sslErrors(const QList<QSslError> &errors) {
    qCWarning(manager) << "SSL Errors on WebSocket Server Port" << errors << this->m_webSockServer->errorString();
}
void QTOZWManager::peerError(QAbstractSocket::SocketError error) {
    qCWarning(manager) << "Client Peer WebSocket Error " << error;
}
void QTOZWManager::peerDisconnected() {
    qCWarning(manager) << "Client Peer WebSocket Disconnected ";
}
#endif

bool QTOZWManager::initilizeReplica(QUrl remote) {
    initilizeBase();
    this->setConnectionType(ConnectionType::Type::Remote);

    this->m_replicaNode = new QRemoteObjectNode(this);
    QObject::connect(this->m_replicaNode, &QRemoteObjectNode::error, this, &QTOZWManager::onReplicaError);

    this->m_log = new QTOZWLog(ConnectionType::Type::Remote, this);
    connect(this, &QTOZWManager::readyChanged, this->m_log, &QTOZWLog::setReady);

    this->m_ozwoptions = new QTOZWOptions(ConnectionType::Type::Remote, this);
    connect(this, &QTOZWManager::readyChanged, this->m_ozwoptions, &QTOZWOptions::setReady);

    this->m_webSockClient = new QWebSocket();
    QObject::connect(this->m_webSockClient, &QWebSocket::connected, this, &QTOZWManager::clientConnected);
    QObject::connect(this->m_webSockClient, &QWebSocket::disconnected, this, &QTOZWManager::clientDisconnected);
    QObject::connect(this->m_webSockClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientError(QAbstractSocket::SocketError)));
#ifndef Q_OS_WASM
    QObject::connect(this->m_webSockClient, &QWebSocket::sslErrors, this, &QTOZWManager::clientSSlErrors);
#endif
    QObject::connect(this->m_webSockClient, &QWebSocket::stateChanged, this, &QTOZWManager::clientStateChanged);

    this->m_webSockIoClient = new WebSocketIoDevice(this->m_webSockClient, true, m_clientAuth, this);
    QObject::connect(this->m_webSockIoClient, &WebSocketIoDevice::authenticated, this, &QTOZWManager::clientAuthenticated);
    QObject::connect(this->m_webSockIoClient, &WebSocketIoDevice::authError, this, &QTOZWManager::clientAuthError);
#ifdef WS_USE_SSL
    // Always use secure connections when available
    QSslConfiguration sslConf;
        QFile certFile(QStringLiteral(":/sslcert/client.crt"));
    if (!certFile.open(QIODevice::ReadOnly))
        qFatal("Can't open client.crt file");
    sslConf.setLocalCertificate(QSslCertificate{certFile.readAll()});

    QFile keyFile(QStringLiteral(":/sslcert/client.key"));
    if (!keyFile.open(QIODevice::ReadOnly))
        qFatal("Can't open client.key file");
    sslConf.setPrivateKey(QSslKey{keyFile.readAll(), QSsl::Rsa});

    sslConf.setPeerVerifyMode(QSslSocket::VerifyPeer);
    webSocket->setSslConfiguration(sslConf);
#endif
    qCDebug(manager) << "Attempting Connection to " << remote;

    this->m_webSockClient->open(remote);

    return true;
}
void QTOZWManager::clientAuthenticated() {
    qCDebug(manager) << "WebSocket Client Authenticated!";
    this->m_replicaNode->addClientSideConnection(this->m_webSockIoClient);
}

void QTOZWManager::clientAuthError(QString error) {
    qCWarning(manager) << "WebSocket Client Authentication Error: " << error;
    this->setReady(false);
}

void QTOZWManager::clientConnected() {
    qCInfo(manager) << "WebSocket Client Connected to " << this->m_webSockClient->peerName() << this->m_webSockClient->peerAddress() << this->m_webSockClient->peerPort();

    this->d_ptr_replica = this->m_replicaNode->acquire<QTOZWManagerReplica>("QTOZWManager");
    QObject::connect(this->d_ptr_replica, &QTOZWManagerReplica::stateChanged, this, &QTOZWManager::onManagerStateChange);

    this->m_ozwoptions->initilizeReplica(this->m_replicaNode);
    QObject::connect(this->m_ozwoptions, &QTOZWOptions::onOptionStateChange, this, &QTOZWManager::onOptionsStateChange);

    this->m_log->initilizeReplica(this->m_replicaNode);
    QObject::connect(this->m_log, &QTOZWLog::onStateChange, this, &QTOZWManager::onLogStateChange);

    this->m_nodeModel = this->m_replicaNode->acquireModel("QTOZW_nodeModel", QtRemoteObjects::InitialAction::PrefetchData);
    QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_nodeModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onNodeInitialized);

    this->m_valueModel= this->m_replicaNode->acquireModel("QTOZW_valueModel", QtRemoteObjects::InitialAction::PrefetchData);
    QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_valueModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onValueInitialized);

    this->m_associationModel= this->m_replicaNode->acquireModel("QTOZW_associationModel", QtRemoteObjects::InitialAction::PrefetchData);
    QObject::connect(qobject_cast<QAbstractItemModelReplica*>(this->m_associationModel), &QAbstractItemModelReplica::initialized, this, &QTOZWManager::onAssociationInitialized);
}
void QTOZWManager::clientDisconnected() {
    qCInfo(manager) << "WebSocket Client Disconnected from " << this->m_webSockClient->peerName() << this->m_webSockClient->peerAddress() << this->m_webSockClient->peerPort();
    this->m_webSockClient->deleteLater();
    this->setReady(false);
}
void QTOZWManager::clientError(QAbstractSocket::SocketError error) {
    qCWarning(manager) << "WebSocket Client Error " << error;
    this->setReady(false);
    emit remoteConnectionStatus(connectionStatus::ConnectionErrorState, error);
}
#ifndef Q_OS_WASM
void QTOZWManager::clientSSlErrors(const QList<QSslError> &errors) {
    qCWarning(manager) << "WebSocket Client SSL Error " << errors;
    this->setReady(false);
}
#endif
void QTOZWManager::clientStateChanged(QAbstractSocket::SocketState state) {
    qCInfo(manager) << "WebSocket Client State Changed: " << state;
    emit remoteConnectionStatus(static_cast<connectionStatus>(state), QAbstractSocket::UnknownSocketError);
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
    qCDebug(manager) << "Manager State Change: " << state;
    this->m_managerState = state;
    if (state == QRemoteObjectReplica::State::Valid) {
        emit remoteConnectionStatus(connectionStatus::GotManagerData, QAbstractSocket::UnknownSocketError);
    } else if (state == QRemoteObjectReplica::State::SignatureMismatch) {
        emit remoteConnectionStatus(connectionStatus::VersionMisMatchError, QAbstractSocket::UnknownSocketError);
    }

    this->checkReplicaReady();
}

void QTOZWManager::onOptionsStateChange(QRemoteObjectReplica::State state) {
    qCDebug(manager) << "Options State Change: " << state;
    this->m_optionsState = state;
    if (state == QRemoteObjectReplica::State::Valid) {
        emit remoteConnectionStatus(connectionStatus::GotOptionData, QAbstractSocket::UnknownSocketError);
    } else if (state == QRemoteObjectReplica::State::SignatureMismatch) {
        emit remoteConnectionStatus(connectionStatus::VersionMisMatchError, QAbstractSocket::UnknownSocketError);
    }
    this->checkReplicaReady();
}

void QTOZWManager::onLogStateChange(QRemoteObjectReplica::State state) {
    qCDebug(manager) << "Log State Change: " << state;
    this->m_logState = state;
    if (state == QRemoteObjectReplica::State::Valid) {
        emit remoteConnectionStatus(connectionStatus::GotLogData, QAbstractSocket::UnknownSocketError);
    } else if (state == QRemoteObjectReplica::State::SignatureMismatch) {
        emit remoteConnectionStatus(connectionStatus::VersionMisMatchError, QAbstractSocket::UnknownSocketError);
    }
    this->checkReplicaReady();
}

void QTOZWManager::onNodeInitialized() {
    this->m_nodeState = true;
    emit remoteConnectionStatus(connectionStatus::GotNodeData, QAbstractSocket::UnknownSocketError);
    this->checkReplicaReady();
}
void QTOZWManager::onValueInitialized() {
    this->m_valuesState = true;
    emit remoteConnectionStatus(connectionStatus::GotValueData, QAbstractSocket::UnknownSocketError);
    this->checkReplicaReady();
}
void QTOZWManager::onAssociationInitialized() {
    this->m_associationsState = true;
    emit remoteConnectionStatus(connectionStatus::GotAssociationData, QAbstractSocket::UnknownSocketError);
    this->checkReplicaReady();
}

void QTOZWManager::checkReplicaReady() {
    qCDebug(manager) << "checkReplicaReady: Manager:" << this->m_managerState << "Options:" << this->m_optionsState << "Nodes:" << this->m_nodeState << "Values" << this->m_valuesState << "Associations:" << this->m_associationsState << "Logs:" << this->m_logState;
    if ((this->m_managerState == QRemoteObjectReplica::State::Valid) &&
            (this->m_optionsState == QRemoteObjectReplica::State::Valid) &&
                (this->m_nodeState == true) &&
                    (this->m_valuesState == true) &&
                        (this->m_associationsState == true) &&
                            (this->m_logState == QRemoteObjectReplica::State::Valid)) {
        /* have to connect all the d_ptr SIGNALS to our SIGNALS now */
        qCInfo(manager) << "checkReplicaReady is Ready!";
        connectSignals();
        setReady(true);
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
#ifndef Q_OS_WASM
    if (this->getConnectionType() == ConnectionType::Type::Local) {
        return this->d_ptr_internal->getNodeModel();
    } else
#endif
    {
        return this->m_nodeModel;
    }
}
QAbstractItemModel *QTOZWManager::getValueModel() {
#ifndef Q_OS_WASM
    if (this->getConnectionType() == ConnectionType::Type::Local) {
        return this->d_ptr_internal->getValueModel();
    } else 
#endif
    {
        return this->m_valueModel;
    }

}
QAbstractItemModel *QTOZWManager::getAssociationModel() {
#ifndef Q_OS_WASM
    if (this->getConnectionType() == ConnectionType::Type::Local) {
        return this->d_ptr_internal->getAssociationModel();
    } else 
#endif
    {
        return this->m_associationModel;
    }
}

QTOZWOptions *QTOZWManager::getOptions() {
    return this->m_ozwoptions;
}

QTOZWLog *QTOZWManager::getLog() {
        return this->m_log;
}

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
    CONNECT_DPTR(nodeGroupChanged);
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

bool QTOZWManager::open(QString serialPort) {
    CALL_DPTR_RTN(open(serialPort), bool, false);
}
bool QTOZWManager::close() {
    CALL_DPTR_RTN(close(), bool, false);
}
bool QTOZWManager::refreshNodeInfo(quint8 _node) {
    CALL_DPTR_RTN(refreshNodeInfo(_node), bool, false);
}
bool QTOZWManager::requestNodeState(quint8 _node) {
    CALL_DPTR_RTN(requestNodeState(_node), bool, false);
}
bool QTOZWManager::requestNodeDynamic(quint8 _node) {
    CALL_DPTR_RTN(requestNodeDynamic(_node), bool, false);
}
bool QTOZWManager::setConfigParam(quint8 _node, quint8 _param, qint32 _value, quint8 const _size) {
    CALL_DPTR_RTN(setConfigParam(_node, _param, _value, _size), bool, false);
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
    CALL_DPTR_RTN(cancelControllerCommand(), bool, false);
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
    CALL_DPTR_RTN(addNode(_doSecure), bool, false);
}
bool QTOZWManager::removeNode() {
    CALL_DPTR_RTN(removeNode(), bool, false);
}
bool QTOZWManager::removeFailedNode(quint8 _node) {
    CALL_DPTR_RTN(removeFailedNode(_node), bool, false);
}
bool QTOZWManager::hasNodeFailed(quint8 _node) {
    CALL_DPTR_RTN(hasNodeFailed(_node), bool, false);
}
bool QTOZWManager::requestNodeNeighborUpdate(quint8 _node) {
    CALL_DPTR_RTN(requestNodeNeighborUpdate(_node), bool, false);
}
bool QTOZWManager::assignReturnRoute(quint8 _node) {
    CALL_DPTR_RTN(assignReturnRoute(_node), bool, false);
}
bool QTOZWManager::deleteAllReturnRoute(quint8 _node) {
    CALL_DPTR_RTN(deleteAllReturnRoute(_node), bool, false);
}
bool QTOZWManager::sendNodeInformation(quint8 _node) {
    CALL_DPTR_RTN(sendNodeInformation(_node), bool, false);
}
bool QTOZWManager::replaceFailedNode(quint8 _node) {
    CALL_DPTR_RTN(replaceFailedNode(_node), bool, false)
}
bool QTOZWManager::requestNetworkUpdate(quint8 _node) {
    CALL_DPTR_RTN(requestNetworkUpdate(_node), bool, false);
}
QString QTOZWManager::GetMetaData(quint8 _node, QTOZWManagerSource::QTOZWMetaDataField _field) {
    if (!this->isReady()) return QString();
#ifndef Q_OS_WASM
    if (this->getConnectionType() == ConnectionType::Type::Local) {
        return this->d_ptr_internal->GetMetaData(_node, _field); \
    } else 
#endif
    {
        QRemoteObjectPendingReply<QString> res = this->d_ptr_replica->GetMetaData(_node, static_cast<QTOZWManagerReplica::QTOZWMetaDataField>(_field));
        res.waitForFinished();
        return res.returnValue();
    }
}

QByteArray QTOZWManager::GetMetaDataProductPic(quint8 _node) {
    CALL_DPTR_RTN(GetMetaDataProductPic(_node), QByteArray, QByteArray());
}

QString QTOZWManager::GetNodeQueryStage(const quint8 _node) {
    CALL_DPTR_RTN(GetNodeQueryStage(_node), QString, QString());
}
NodeStatistics QTOZWManager::GetNodeStatistics(const quint8 _node) {
    if (!this->isReady()) {
        NodeStatistics empty;
        return empty;
    }
#ifndef Q_OS_WASM
    if (this->getConnectionType() == ConnectionType::Type::Local) {
        return this->d_ptr_internal->GetNodeStatistics(_node); 
    } else 
#endif
    {
        QRemoteObjectPendingReply<NodeStatistics> res = this->d_ptr_replica->GetNodeStatistics(_node);
        res.waitForFinished();
        return res.returnValue();
    }
}

DriverStatistics QTOZWManager::GetDriverStatistics() {
    if (!this->isReady()) {
        DriverStatistics empty;
        return empty;
    }
#ifndef Q_OS_WASM
    if (this->getConnectionType() == ConnectionType::Type::Local) {
        return this->d_ptr_internal->GetDriverStatistics(); 
    } else 
#endif
    {
        QRemoteObjectPendingReply<DriverStatistics> res = this->d_ptr_replica->GetDriverStatistics();
        res.waitForFinished();
        return res.returnValue();
    }
}

QVector<quint8> QTOZWManager::GetNodeNeighbors(quint8 const _node) {
    CALL_DPTR_RTN(GetNodeNeighbors(_node), QVector<quint8>, QVector<quint8>());
}

bool QTOZWManager::IsNodeFailed(const quint8 _node) {
    CALL_DPTR_RTN(IsNodeFailed(_node), bool, false);
}
bool QTOZWManager::checkLatestConfigFileRevision(quint8 const _node) {
    CALL_DPTR_RTN(checkLatestConfigFileRevision(_node), bool, false);
}
bool QTOZWManager::checkLatestMFSRevision() {
    CALL_DPTR_RTN(checkLatestMFSRevision(), bool, false);
}
bool QTOZWManager::downloadLatestConfigFileRevision(quint8 const _node) {
    CALL_DPTR_RTN(downloadLatestConfigFileRevision(_node), bool, false);
}
bool QTOZWManager::downloadLatestMFSRevision() {
    CALL_DPTR_RTN(downloadLatestMFSRevision(), bool, false);
}

QString QTOZWManager::getCommandClassString(quint8 const _cc) {
    CALL_DPTR_RTN(getCommandClassString(_cc), QString, QString());
}
quint8 QTOZWManager::getCommandClassVersion(quint8 const _node, quint8 const _cc) {
    CALL_DPTR_RTN(getCommandClassVersion(_node, _cc), quint8, 0);
}
QString QTOZWManager::getVersionAsString() {
    CALL_DPTR_RTN(getVersionAsString(), QString, QString());
}
QString QTOZWManager::getVersionLongAsString() {
    CALL_DPTR_RTN(getVersionLongAsString(), QString, QString());
}
quint8 QTOZWManager::getControllerNodeId() {
    CALL_DPTR_RTN(getControllerNodeId(), quint8, 0);
}
quint8 QTOZWManager::getSucNodeId() {
    CALL_DPTR_RTN(getSucNodeId(), quint8, 0);
}
bool QTOZWManager::isPrimaryController() {
    CALL_DPTR_RTN(isPrimaryController(), bool, false);
}
bool QTOZWManager::isStaticUpdateController() {
    CALL_DPTR_RTN(isStaticUpdateController(), bool, false);
}
bool QTOZWManager::isBridgeController() {
    CALL_DPTR_RTN(isBridgeController(), bool, false);
}
bool QTOZWManager::hasExtendedTXStatus() {
    CALL_DPTR_RTN(hasExtendedTXStatus(), bool, false);
}
QString QTOZWManager::getLibraryVersion() {
    CALL_DPTR_RTN(getLibraryVersion(), QString, QString());
}
QString QTOZWManager::getLibraryTypeName() {
    CALL_DPTR_RTN(getLibraryTypeName(), QString, QString());
}
quint32 QTOZWManager::getSendQueueCount() {
    CALL_DPTR_RTN(getSendQueueCount(), quint32, 0);
}
QString QTOZWManager::getControllerPath() {
    CALL_DPTR_RTN(getControllerPath(), QString, QString());
}
qint32 QTOZWManager::getPollInterval() {
    CALL_DPTR_RTN(getPollInterval(), qint32, 0);
}
void QTOZWManager::setPollInterval(qint32 interval, bool intervalBetweenPolls) {
    CALL_DPTR(setPollInterval(interval, intervalBetweenPolls));
}
void QTOZWManager::syncroniseNodeNeighbors(quint8 node) {
    CALL_DPTR(syncroniseNodeNeighbors(node));
}
bool QTOZWManager::refreshValue(quint64 vidKey) {
    CALL_DPTR_RTN(refreshValue(vidKey), bool, false);
}
bool QTOZWManager::AddAssociation (quint8 const _nodeId, quint8 const _groupIdx, QString const target) {
    CALL_DPTR_RTN(AddAssociation(_nodeId, _groupIdx, target), bool, false);
}
bool QTOZWManager::RemoveAssociation (quint8 const _nodeId, quint8 const _groupIdx, QString const target) {
    CALL_DPTR_RTN(RemoveAssociation(_nodeId, _groupIdx, target), bool, false);
}
bool QTOZWManager::enablePoll(quint64 vidKey, quint8 intensity) {
    CALL_DPTR_RTN(enablePoll(vidKey, intensity), bool, false);
}
bool QTOZWManager::disablePoll(quint64 vidKey) {
    CALL_DPTR_RTN(disablePoll(vidKey), bool, false);
}
QString QTOZWManager::getInstanceLabel(quint64 vidKey) {
    CALL_DPTR_RTN(getInstanceLabel(vidKey), QString, QString());
}
QString QTOZWManager::getValueLabel(quint64 vidKey) {
    CALL_DPTR_RTN(getValueLabel(vidKey), QString, QString());
}
QString QTOZWManager::getValueUnits(quint64 vidKey) {
    CALL_DPTR_RTN(getValueUnits(vidKey), QString, QString());
}
QString QTOZWManager::getValueHelp(quint64 vidKey) {
    CALL_DPTR_RTN(getValueHelp(vidKey), QString, QString());
}
qint32 QTOZWManager::getValueMin(quint64 vidKey) {
    CALL_DPTR_RTN(getValueMin(vidKey), qint32, 0);
}
qint32 QTOZWManager::getValueMax(quint64 vidKey) {
    CALL_DPTR_RTN(getValueMax(vidKey), qint32, 0);
}
bool QTOZWManager::isValueReadOnly(quint64 vidKey) {
    CALL_DPTR_RTN(isValueReadOnly(vidKey), bool, false);
}
bool QTOZWManager::isValueWriteOnly(quint64 vidKey) {
    CALL_DPTR_RTN(isValueWriteOnly(vidKey), bool, false);
}
bool QTOZWManager::isValueSet(quint64 vidKey) {
    CALL_DPTR_RTN(isValueSet(vidKey), bool, false);
}
bool QTOZWManager::isValuePolled(quint64 vidKey) {
    CALL_DPTR_RTN(isValuePolled(vidKey), bool, false);
}
bool QTOZWManager::isValueValid(quint64 vidKey) {
    CALL_DPTR_RTN(isValueValid(vidKey), bool, false);
}
quint8 QTOZWManager::getNodeId(quint64 vidKey) {
    CALL_DPTR_RTN(getNodeId(vidKey), quint8, 0);
}
quint8 QTOZWManager::getInstance(quint64 vidKey) {
    CALL_DPTR_RTN(getInstance(vidKey), quint8, 0);
}
ValueTypes::valueGenre QTOZWManager::getGenre(quint64 vidKey) {
    CALL_DPTR_RTN(getGenre(vidKey), ValueTypes::valueGenre, ValueTypes::valueGenre::InvalidGenre) ;
}
quint8 QTOZWManager::getComamndClass(quint64 vidKey) {
    CALL_DPTR_RTN(getComamndClass(vidKey), quint8, 0);
}
quint16 QTOZWManager::getIndex(quint64 vidKey) {
    CALL_DPTR_RTN(getIndex(vidKey), quint16, 0);
}
ValueTypes::valueType QTOZWManager::getType(quint64 vidKey) {
    CALL_DPTR_RTN(getType(vidKey), ValueTypes::valueType, ValueTypes::valueType::InvalidType);    
}

void QTOZWManager::setOZWConfigPath(QString path) {
        this->m_ozwconfigpath = path;
}
void QTOZWManager::setOZWUserPath(QString path) {
        this->m_ozwuserpath = path;
}

