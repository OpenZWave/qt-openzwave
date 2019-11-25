//-----------------------------------------------------------------------------
//
//	qtozwmanager_p.cpp
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

#include "qtozwmanager_p.h"
#include "qt-openzwave/qtozwmanager.h"


QTOZWManager_Internal::QTOZWManager_Internal(QObject *parent)
    : QTOZWManagerSimpleSource (parent)
{
    this->setObjectName("QTOZW_Manager");
    try {
        qCDebug(manager) << "OpenZWave Options Class Creating";
        this->m_options = new QTOZWOptions_Internal(this);
        qCDebug(manager) << "OpenZWave Options Class Set";
    } catch (OpenZWave::OZWException &e) {
        qCWarning(manager) << "Failed to Load Options Class" << QString(e.GetMsg().c_str());
        return;
    }
    this->m_nodeModel = new QTOZW_Nodes_internal(this);
    QObject::connect(this->m_nodeModel, &QTOZW_Nodes_internal::dataChanged, this, &QTOZWManager_Internal::pvt_nodeModelDataChanged);
    this->m_valueModel = new QTOZW_ValueIds_internal(this);
    QObject::connect(this->m_valueModel, &QTOZW_ValueIds_internal::dataChanged, this, &QTOZWManager_Internal::pvt_valueModelDataChanged);
    this->m_associationsModel = new QTOZW_Associations_internal(this);

    qCDebug(manager) << "Models Created";

}

bool QTOZWManager_Internal::open(QString SerialPort)
{
    emit this->starting();
    OpenZWave::Options::Get()->Lock();
    try {
        this->m_manager = OpenZWave::Manager::Create();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Load Manager Class" << QString(e.GetMsg().c_str());
        return false;
    }
    qCDebug(manager) << "OpenZWave Manager Instance Created";
    try {
        if (this->m_manager->AddWatcher( OZWNotification::processNotification, this ) != true) {
            emit this->error(QTOZWManagerErrorCodes::setupFailed);
            this->setErrorString("Failed to Add Notification Callback");
            qCWarning(manager) << "Failed to Add Notification Callback";
            return false;
        }
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Add Notification Callback " << QString(e.GetMsg().c_str());
        return false;
    }
    qCDebug(manager) << "OpenZWave Watcher Registered";

    /* setup all our Connections to the Notifications
    ** these are set as QueuedConnections as the Notifications can happen on a different thread
    ** to our main QT thread thats running this manager
    */
    QObject::connect(OZWNotification::Get(), &OZWNotification::valueAdded, this, &QTOZWManager_Internal::pvt_valueAdded, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::valueRemoved, this, &QTOZWManager_Internal::pvt_valueRemoved, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::valueChanged, this, &QTOZWManager_Internal::pvt_valueChanged, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::valueRefreshed, this, &QTOZWManager_Internal::pvt_valueRefreshed, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::valuePollingEnabled, this, &QTOZWManager_Internal::pvt_valuePollingEnabled, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::valuePollingDisabled, this, &QTOZWManager_Internal::pvt_valuePollingDisabled, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeGroupChanged, this, &QTOZWManager_Internal::pvt_nodeGroupChanged, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeNew, this, &QTOZWManager_Internal::pvt_nodeNew, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeAdded, this, &QTOZWManager_Internal::pvt_nodeAdded, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeRemoved, this, &QTOZWManager_Internal::pvt_nodeRemoved, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeReset, this, &QTOZWManager_Internal::pvt_nodeReset, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeNaming, this, &QTOZWManager_Internal::pvt_nodeNaming, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeEvent, this, &QTOZWManager_Internal::pvt_nodeEvent, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeProtocolInfo, this, &QTOZWManager_Internal::pvt_nodeProtocolInfo, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeEssentialNodeQueriesComplete, this, &QTOZWManager_Internal::pvt_nodeEssentialNodeQueriesComplete, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::nodeQueriesComplete, this, &QTOZWManager_Internal::pvt_nodeQueriesComplete, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverReady, this, &QTOZWManager_Internal::pvt_driverReady, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverFailed, this, &QTOZWManager_Internal::pvt_driverFailed, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverReset, this, &QTOZWManager_Internal::pvt_driverReset, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverRemoved, this, &QTOZWManager_Internal::pvt_driverRemoved, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverAllNodesQueriedSomeDead, this, &QTOZWManager_Internal::pvt_driverAllNodesQueriedSomeDead, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverAwakeNodesQueried, this, &QTOZWManager_Internal::pvt_driverAwakeNodesQueried, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::driverAllNodesQueried, this, &QTOZWManager_Internal::pvt_driverAllNodesQueried, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::controllerCommand, this, &QTOZWManager_Internal::pvt_controllerCommand, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::ozwNotification, this, &QTOZWManager_Internal::pvt_ozwNotification, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::ozwUserAlert, this, &QTOZWManager_Internal::pvt_ozwUserAlert, Qt::QueuedConnection);
    QObject::connect(OZWNotification::Get(), &OZWNotification::manufacturerSpecificDBReady, this, &QTOZWManager_Internal::pvt_manufacturerSpecificDBReady, Qt::QueuedConnection);

    qCDebug(manager) << "Notification Signals Setup";

    try {
        if (this->m_manager->AddDriver( SerialPort.toStdString()) != true) {
            emit this->error(QTOZWManagerErrorCodes::setupFailed);
            this->setErrorString("Failed to Add Serial Port");
            qCWarning(manager) << "Failed to Add Serial Port";
            return false;
        }
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Add Serial Port: " << QString(e.GetMsg().c_str());
        return false;
    }
    qCDebug(manager) << "AddDriver Completed";
    this->m_SerialPort = SerialPort;
    return true;
}

bool QTOZWManager_Internal::close() {
    try {
        if (this->m_manager->RemoveDriver(this->m_SerialPort.toStdString())) {
            qCDebug(manager) << "Driver Removed for " << this->m_SerialPort;
        } else {
            qCWarning(manager) << "Couldn't Remove Driver for " << this->m_SerialPort;
        }
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Remove Driver: " << QString(e.GetMsg().c_str());
        return false;
    }
    try {
        if (this->m_manager->RemoveWatcher(OZWNotification::processNotification, this ) != true) {
            emit this->error(QTOZWManagerErrorCodes::setupFailed);
            this->setErrorString("Failed to Remove Notification Callback");
            qCWarning(manager) << "Failed to Remove Notification Callback";
            return false;
        }
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Remove Notification Callback " << QString(e.GetMsg().c_str());
        return false;
    }
    if (OZWNotification::Get()->disconnect() != true) {
        emit this->error(QTOZWManagerErrorCodes::setupFailed);
        this->setErrorString("Failed to Disconnect Notification Signals");
        qCWarning(manager) << "Failed to Disconnect Notification Signals";
        return false;
    }
    qCDebug(manager) << "OZW Serial Port Closed";
    this->m_SerialPort = QString();
    return true;
}

bool QTOZWManager_Internal::refreshNodeInfo(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RefreshNodeInfo(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

bool QTOZWManager_Internal::requestNodeState(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RequestNodeState(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

bool QTOZWManager_Internal::requestNodeDynamic(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RequestNodeDynamic(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

bool QTOZWManager_Internal::setConfigParam(quint8 _node, quint8 _param, qint32 _value, quint8 const _size) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->SetConfigParam(this->homeId(), _node, _param, _value, _size);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

void QTOZWManager_Internal::requestConfigParam(quint8 _node, quint8 _param) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return;
    try {
        this->m_manager->RequestConfigParam(this->homeId(), _node, _param);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}

void QTOZWManager_Internal::requestAllConfigParam(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return;
    try {
        this->m_manager->RequestAllConfigParams(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}

void QTOZWManager_Internal::softResetController() {
    if (!this->checkHomeId())
        return;
    try {
        this->m_manager->SoftReset(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;

}

void QTOZWManager_Internal::hardResetController() {
    if (!this->checkHomeId())
        return;
    try {
        this->m_manager->ResetController(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;

}

bool QTOZWManager_Internal::cancelControllerCommand() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->CancelControllerCommand(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

void QTOZWManager_Internal::testNetworkNode(quint8 _node, quint32 const _count) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return;
    try {
        this->m_manager->TestNetworkNode(this->homeId(), _node, _count);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}
void QTOZWManager_Internal::testNetwork(quint32 const _count) {
    if (!this->checkHomeId() )
        return;
    try {
        this->m_manager->TestNetwork(this->homeId(), _count);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}
void QTOZWManager_Internal::healNetworkNode(quint8 _node, bool _doRR) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return;
    try {
        this->m_manager->HealNetworkNode(this->homeId(), _node, _doRR);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}
void QTOZWManager_Internal::healNetwork(bool _doRR) {
    if (!this->checkHomeId())
        return;
    try {
        this->m_manager->HealNetwork(this->homeId(), _doRR);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}
bool QTOZWManager_Internal::addNode(bool _doSecure) {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->AddNode(this->homeId(), _doSecure);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::removeNode() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->RemoveNode(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::removeFailedNode(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RemoveFailedNode(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::hasNodeFailed(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->HasNodeFailed(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::requestNodeNeighborUpdate(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RequestNodeNeighborUpdate(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::assignReturnRoute(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->AssignReturnRoute(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::deleteAllReturnRoute(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->DeleteAllReturnRoutes(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::sendNodeInformation(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->SendNodeInformation(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::replaceFailedNode(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->ReplaceFailedNode(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::requestNetworkUpdate(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RequestNetworkUpdate(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

QString QTOZWManager_Internal::GetMetaData(quint8 _node, QTOZWMetaDataField _field) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return QString();
    try {
        return QString::fromStdString(this->m_manager->GetMetaData(this->homeId(), _node, static_cast<OpenZWave::Node::MetaDataFields>(_field)));
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}

QByteArray QTOZWManager_Internal::GetMetaDataProductPic(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return QByteArray();
    try {
        this->m_options->GetOptionAsString("ConfigPath");
        QString path(this->m_options->GetOptionAsString("ConfigPath"));
        path.append(QString::fromStdString(this->m_manager->GetMetaData(this->homeId(), _node, OpenZWave::Node::MetaDataFields::MetaData_ProductPic)));
        qCDebug(manager) << "ProductPic Path: " << path;
        if (path == this->m_options->GetOptionAsString("ConfigPath"))
            return QByteArray();
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return QByteArray();
        return file.readAll();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QByteArray();


}

QString QTOZWManager_Internal::GetNodeQueryStage(quint8 const _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return QString();
    try {
        return this->m_manager->GetNodeQueryStage(this->homeId(), _node).c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}


NodeStatistics QTOZWManager_Internal::GetNodeStatistics(quint8 const _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return NodeStatistics();
    try {
        OpenZWave::Node::NodeData nd;
        NodeStatistics ns;
        this->m_manager->GetNodeStatistics(this->homeId(), _node, &nd);
        /* copy */
        ns.hops = nd.m_hops;
        ns.rssi_1 = nd.m_rssi_1;
        ns.rssi_2 = nd.m_rssi_2;
        ns.rssi_3 = nd.m_rssi_3;
        ns.rssi_4 = nd.m_rssi_4;
        ns.rssi_5 = nd.m_rssi_5;
        ns.route_1 = static_cast<quint8>(nd.m_routeUsed[0]);
        ns.route_2 = static_cast<quint8>(nd.m_routeUsed[1]);
        ns.route_3 = static_cast<quint8>(nd.m_routeUsed[2]);
        ns.route_4 = static_cast<quint8>(nd.m_routeUsed[3]);
        ns.txTime = nd.m_txTime;
        ns.quality = nd.m_quality;
        ns.retries = nd.m_retries;
        ns.routeUsed = nd.m_routeUsed;
        ns.sentCount = nd.m_sentCnt;
        ns.ackChannel = nd.m_ackChannel;
        ns.routeSpeed = this->m_manager->GetNodeRouteSpeed(&nd).c_str();
        ns.routeTries = nd.m_routeTries;
        ns.sentFailed = nd.m_sentFailed;
        ns.routeScheme = this->m_manager->GetNodeRouteScheme(&nd).c_str();
        ns.lastTXChannel = nd.m_lastTxChannel;
        ns.lastRequestRTT = nd.m_lastRequestRTT;
        ns.lastResponseRTT = nd.m_lastResponseRTT;
        ns.receivedPackets = nd.m_receivedCnt;
        ns.lastFailedLinkTo = nd.m_lastFailedLinkTo;
        ns.averageRequestRTT = nd.m_averageRequestRTT;
        /*                                                                 2019-11-06 21:11:02:549  */
        ns.lastSentTimeStamp = QDateTime::fromString(QString::fromStdString(nd.m_sentTS).trimmed(), "yyyy-MM-dd HH:mm:ss:zzz");
        ns.lastReceivedTimeStamp = QDateTime::fromString(QString::fromStdString(nd.m_receivedTS).trimmed(), "yyyy-MM-dd HH:mm:ss:zzz");
        ns.averageResponseRTT = nd.m_averageResponseRTT;
        ns.lastFailedLinkFrom = nd.m_lastFailedLinkFrom;
        ns.receivedDupPackets = nd.m_receivedDups;
        ns.extendedTXSupported = nd.m_txStatusReportSupported;
        ns.receivedUnsolicited = nd.m_receivedUnsolicited;
        return ns;
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return NodeStatistics();

}

DriverStatistics QTOZWManager_Internal::GetDriverStatistics() {
        if (!this->checkHomeId())
        return DriverStatistics();
    try {
        OpenZWave::Driver::DriverData dd;
        DriverStatistics ds;
        this->m_manager->GetDriverStatistics(this->homeId(), &dd);
        /* copy */
        ds.m_SOFCnt = dd.m_SOFCnt;
        ds.m_ACKWaiting = dd.m_ACKWaiting;
        ds.m_readAborts = dd.m_readAborts;
        ds.m_badChecksum = dd.m_badChecksum;
        ds.m_readCnt = dd.m_readCnt;
        ds.m_writeCnt = dd.m_writeCnt;
        ds.m_CANCnt = dd.m_CANCnt;
        ds.m_NAKCnt = dd.m_NAKCnt;
        ds.m_ACKCnt = dd.m_ACKCnt;
        ds.m_OOFCnt = dd.m_OOFCnt;
        ds.m_dropped = dd.m_dropped;
        ds.m_retries = dd.m_retries;
        ds.m_callbacks = dd.m_callbacks;
        ds.m_badroutes = dd.m_badroutes;
        ds.m_noack = dd.m_noack;
        ds.m_netbusy = dd.m_netbusy;
        ds.m_notidle = dd.m_notidle;
        ds.m_txverified = dd.m_txverified;
        ds.m_nondelivery = dd.m_nondelivery;
        ds.m_routedbusy = dd.m_routedbusy;
        ds.m_broadcastReadCnt = dd.m_broadcastReadCnt;
        ds.m_broadcastWriteCnt = dd.m_broadcastWriteCnt;
        return ds;
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return DriverStatistics();
}



QVector<quint8> QTOZWManager_Internal::GetNodeNeighbors(quint8 const _node) {
        if (!this->checkHomeId() || !this->checkNodeId(_node))
        return QVector<quint8>();
    try {
        uint8 *neighbours;
        int size = this->m_manager->GetNodeNeighbors(this->homeId(), _node, &neighbours);
        QVector<quint8> data;
        for (int i = 0; i < size; i++) {
            data.append(neighbours[i]);
        }
        delete[] neighbours;
        return data;
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QVector<quint8>();
}

bool QTOZWManager_Internal::IsNodeFailed(quint8 const _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->IsNodeFailed(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}


bool QTOZWManager_Internal::checkLatestConfigFileRevision(quint8 const _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->checkLatestConfigFileRevision(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

bool QTOZWManager_Internal::checkLatestMFSRevision() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->checkLatestMFSRevision(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

bool QTOZWManager_Internal::downloadLatestConfigFileRevision(quint8 const _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->downloadLatestConfigFileRevision(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

bool QTOZWManager_Internal::downloadLatestMFSRevision() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->downloadLatestMFSRevision(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}

QString QTOZWManager_Internal::getCommandClassString(quint8 const _cc) {
    try {
        return this->m_manager->GetCommandClassName(_cc).c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}

QString QTOZWManager_Internal::getVersionAsString() {
    try {
        return this->m_manager->getVersionAsString().c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}
QString QTOZWManager_Internal::getVersionLongAsString() {
    try {
        return this->m_manager->getVersionLongAsString().c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}
quint8 QTOZWManager_Internal::getControllerNodeId() {
    if (!this->checkHomeId())
        return 0;
    try {
        return this->m_manager->GetControllerNodeId(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return 0;
}
quint8 QTOZWManager_Internal::getSucNodeId() {
    if (!this->checkHomeId())
        return 0;
    try {
        return this->m_manager->GetSUCNodeId(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return 0;
}
bool QTOZWManager_Internal::isPrimaryController() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->IsPrimaryController(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::isStaticUpdateController() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->IsStaticUpdateController(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::isBridgeController() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->IsBridgeController(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::hasExtendedTXStatus() {
    if (!this->checkHomeId())
        return false;
    try {
        return this->m_manager->HasExtendedTxStatus(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
QString QTOZWManager_Internal::getLibraryVersion() {
    if (!this->checkHomeId())
        return QString();
    try {
        return this->m_manager->GetLibraryVersion(this->homeId()).c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}
QString QTOZWManager_Internal::getLibraryTypeName() {
    if (!this->checkHomeId())
        return QString();
    try {
        return this->m_manager->GetLibraryTypeName(this->homeId()).c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}
quint32 QTOZWManager_Internal::getSendQueueCount() {
    if (!this->checkHomeId())
        return 0;
    try {
        return this->m_manager->GetSendQueueCount(this->homeId());
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return 0;
}
QString QTOZWManager_Internal::getControllerPath() {
    if (!this->checkHomeId())
        return QString();
    try {
        return this->m_manager->GetControllerPath(this->homeId()).c_str();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return QString();
}
qint32 QTOZWManager_Internal::getPollInterval() {
    try {
        return this->m_manager->GetPollInterval();
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return 0;
}
void QTOZWManager_Internal::setPollInterval(qint32 interval, bool intervalBetweenPolls) {
    if (!this->checkHomeId())
        return;
    try {
        this->m_manager->SetPollInterval(interval, intervalBetweenPolls);
        return;
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}
void QTOZWManager_Internal::syncroniseNodeNeighbors(quint8 node) {
    if (!this->checkHomeId())
        return;
    if (!this->checkNodeId(node))
        return;
    try {
        this->m_manager->SyncronizeNodeNeighbors(this->homeId(), node);
        return;
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return;
}


QTOZW_Nodes *QTOZWManager_Internal::getNodeModel() {
    return static_cast<QTOZW_Nodes *>(this->m_nodeModel);
}

QTOZW_ValueIds *QTOZWManager_Internal::getValueModel() {
    return static_cast<QTOZW_ValueIds *>(this->m_valueModel);
}

QTOZW_Associations *QTOZWManager_Internal::getAssociationModel() {
    return static_cast<QTOZW_Associations *>(this->m_associationsModel);
}

QTOZWOptions_Internal *QTOZWManager_Internal::getOptions() {
    return this->m_options;
}

QTOZW_Log_Internal *QTOZWManager_Internal::getLogModel() {
    return this->m_logModel;
}
bool QTOZWManager_Internal::setLogModel(QTOZW_Log_Internal *t_logModel) {
    this->m_logModel= t_logModel;
    return true;
}




bool QTOZWManager_Internal::checkHomeId() {
    if (this->m_manager == nullptr) {
        emit this->error(QTOZWManagerErrorCodes::Manager_Not_Started);
        this->setErrorString("Manager Not Started");
        return false;
    }
    if (this->homeId() == 0) {
        emit this->error(QTOZWManagerErrorCodes::homeId_Invalid);
        this->setErrorString("Invalid homeID");
        return false;
    }
    return true;
}
bool QTOZWManager_Internal::checkNodeId(quint8 _node) {
    if (!this->m_validNodes.contains(_node)) {
        emit this->error(QTOZWManagerErrorCodes::nodeId_Invalid);
        this->setErrorString("Invalid nodeID");
        return false;
    }
    return true;
}

bool QTOZWManager_Internal::checkValueKey(quint64 _vidKey) {
    if (!this->m_validValues.contains(_vidKey)) {
        emit this->error(QTOZWManagerErrorCodes::valueIDKey_Invalid);
        this->setErrorString("Invalid ValueID Key");
        return false;
    }
    return true;
}

bool QTOZWManager_Internal::convertValueID(quint64 vidKey) {
    OpenZWave::ValueID vid(this->homeId(), vidKey);
    switch (vid.GetType()) {
        case OpenZWave::ValueID::ValueType_Bool:
        {
            bool value;
            this->m_manager->GetValueAsBool(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(value), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Bool, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Byte:
        {
            quint8 value;
            this->m_manager->GetValueAsByte(vid, &value);
            /* QT has a habbit of treating quint8 as char... so cast it to 32 to get around that */
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(static_cast<quint32>(value)), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Byte, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Decimal:
        {
            std::string value;
            this->m_manager->GetValueAsString(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(QString(value.c_str()).toFloat()), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Decimal, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Int:
        {
            qint32 value;
            this->m_manager->GetValueAsInt(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(value), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Int, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_List:
        {
            std::vector<std::string> items;
            std::vector<int32> values;
            this->m_manager->GetValueListItems(vid, &items);
            this->m_manager->GetValueListValues(vid, &values);
            if (items.size() != values.size()) {
                qCWarning(manager) << "ValueList Item Size Does not equal Value Size";
            } else {
                std::vector<std::string>::iterator it;
                size_t i = 0;
                QTOZW_ValueIDList vidlist;
                for(it = items.begin(); it != items.end(); it++) {
                    vidlist.labels.push_back(QString::fromStdString(*it));
                    vidlist.values.push_back(static_cast<unsigned int>(values.at(i)));
                    i++;
                }
                std::string selectedItem;
                this->m_manager->GetValueListSelection(vid, &selectedItem);
                vidlist.selectedItem = QString::fromStdString(selectedItem);
                this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(vidlist), true);
            }
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::List, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Schedule:
        {
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Schedule, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Short:
        {
            int16_t value;
            this->m_manager->GetValueAsShort(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(value), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Short, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_String:
        {
            std::string value;
            this->m_manager->GetValueAsString(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(QString(value.c_str())), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::String, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Button:
        {
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Button, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Raw:
        {
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Raw, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
        case OpenZWave::ValueID::ValueType_BitSet:
        {
            quint8 bssize;
            qint32 bsmask;
            this->m_manager->GetBitSetSize(vid, &bssize);
            this->m_manager->GetBitMask(vid, &bsmask);
            QTOZW_ValueIDBitSet vidbs;
            vidbs.mask.resize(bssize * 8);
            for (int i = 1; i < bssize * 8; ++i) {
                vidbs.mask[i] = (bsmask & (1 << i));
            }
            vidbs.values.resize(bssize * 8);
            for (quint8 i = 1; i <= bssize * 8; ++i) {
                /* OZW is 1 base - QT is 0 base. */
                if (vidbs.mask.at(i-1)) {
                    bool value;
                    if (this->m_manager->GetValueAsBitSet(vid, i, &value)) {
                        vidbs.values[i-1] = value;
                        vidbs.label[i-1] = QString::fromStdString(this->m_manager->GetValueLabel(vid, i));
                        vidbs.help[i-1] = QString::fromStdString(this->m_manager->GetValueHelp(vid, i));
                    }
                }
            }
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(vidbs), true);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::BitSet, true);
            this->m_valueModel->finishTransaction(vidKey);
            return true;
        }
    }
    return false;
}


void QTOZWManager_Internal::pvt_valueAdded(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valueAdded:" << vidKey;
    if (!this->m_validValues.contains(vidKey))
        this->m_validValues.push_back(vidKey);

    this->m_valueModel->addValue(vidKey);
#if 0
    /* XXX TODO: Need to check/implement these functions on ValueModel */
    bool enablePoll(quint64 vidKey, uint8 intensity);
    bool disablePoll(quint64 vidKey);
    bool isPolled(quint64 vidKey);
    bool setPollIntensity(quint64 vidKey, quint8 intensity);
    quint8 getPollIntensity(quint64 vidKey);

    QString getInstanceLabel(quint64 vidKey);
    QString getInstanceLabel(quint8 node, quint8 cc, quint8 instance);

    bool refreshValue(quint64 vidKey);
    bool setChangedVerified(quint64 vidKey);
    bool getChangedVerified(quint64 vidKey);
#endif
    try {
        OpenZWave::ValueID vid(this->homeId(), vidKey);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Label, this->m_manager->GetValueLabel(vid).c_str(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Instance, vid.GetInstance(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::CommandClass, vid.GetCommandClassId(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Index, vid.GetIndex(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node, vid.GetNodeId(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Genre, vid.GetGenre(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Help, this->m_manager->GetValueHelp(vid).c_str(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Units, this->m_manager->GetValueUnits(vid).c_str(), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Min, this->m_manager->GetValueMin(vid), true);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Max, this->m_manager->GetValueMax(vid), true);
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ReadOnly, this->m_manager->IsValueReadOnly(vid), true);
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::WriteOnly, this->m_manager->IsValueWriteOnly(vid), true);
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValueSet, this->m_manager->IsValueSet(vid), true);
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValuePolled, this->m_manager->IsValuePolled(vid), true);
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ChangeVerified, this->m_manager->GetChangeVerified(vid), true);
        this->m_valueModel->finishTransaction(vidKey);
        this->convertValueID(vidKey);
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->valueAdded(vidKey);
}
void QTOZWManager_Internal::pvt_valueRemoved(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valueRemoved: " << vidKey;
    emit this->valueRemoved(vidKey);
    if (this->m_validValues.contains(vidKey))
        this->m_validValues.removeAll(vidKey);
    this->m_valueModel->delValue(vidKey);
}
void QTOZWManager_Internal::pvt_valueChanged(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valueChanged: " << vidKey;
    this->convertValueID(vidKey);
    emit this->valueChanged(vidKey);
}
void QTOZWManager_Internal::pvt_valueRefreshed(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valueRefreshed: " << vidKey;

    this->convertValueID(vidKey);
    emit this->valueRefreshed(vidKey);
}
void QTOZWManager_Internal::pvt_valuePollingEnabled(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valuePollingEnabled " << vidKey;
    this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValuePolled, true, true);
    this->m_valueModel->finishTransaction(vidKey);

}
void QTOZWManager_Internal::pvt_valuePollingDisabled(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valuePollingDisabled " << vidKey;
    this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValuePolled, false, true);
    this->m_valueModel->finishTransaction(vidKey);

}
void QTOZWManager_Internal::pvt_nodeGroupChanged(quint8 node, quint8 group)
{
    qCDebug(notifications) << "Notification pvt_nodeGroupChanged " << node << " Group: " << group;

    if (this->m_associationsModel->getassocationRow(node, group) == -1) {
        this->m_associationsModel->addGroup(node, group);
        this->m_associationsModel->setGroupData(node, group, QTOZW_Associations::associationColumns::MaxAssocations, this->m_manager->GetMaxAssociations(this->homeId(), node, group));
        this->m_associationsModel->setGroupFlags(node, group, QTOZW_Associations::associationFlags::isMultiInstance, this->m_manager->IsMultiInstance(this->homeId(), node, group));
        this->m_associationsModel->setGroupData(node, group, QTOZW_Associations::associationColumns::GroupName, this->m_manager->GetGroupLabel(this->homeId(), node, group).c_str());
    }

    OpenZWave::InstanceAssociation *ia;
    quint32 count = this->m_manager->GetAssociations(this->homeId(), node, group, &ia);
    for (quint32 i = 0;  i < count; i++) {
        if (!this->m_associationsModel->findAssociation(node, group, ia[i].m_nodeId, ia[i].m_instance)) {
            this->m_associationsModel->addAssociation(node, group, ia[i].m_nodeId, ia[i].m_instance);
        }
    }

    QStringList list = this->m_associationsModel->getassocationData(node, group, QTOZW_Associations::associationColumns::Members).toStringList();
    QStringList removeitems;
    for (int i = 0; i < list.size(); ++i) {
        QString member = list.at(i);
        int targetnode = member.split(":")[0].toInt();
        int targetinstance = member.split(":")[1].toInt();
        bool found = false;
        for (quint32 j = 0; j < count; j++) {
            if (targetnode == ia[i].m_nodeId && targetinstance == ia[i].m_instance)
                found = true;
        }
        if (found == false) {
            this->m_associationsModel->delAssociation(node, group, static_cast<quint8>(targetnode), static_cast<quint8>(targetinstance));
        }
    }

    if (ia != nullptr)
        delete [] ia;
}
void QTOZWManager_Internal::pvt_nodeNew(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeNew " << node;
    if (!this->m_validNodes.contains(node))
        this->m_validNodes.push_back(node);
    this->m_nodeModel->addNode(node);
    try {
        QVariant data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);
        this->m_nodeModel->finishTransaction(node);
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }

    emit this->nodeNew(node);
}
void QTOZWManager_Internal::pvt_nodeAdded(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeAdded " << node;
    if (!this->m_validNodes.contains(node))
        this->m_validNodes.push_back(node);

    this->m_nodeModel->addNode(node);
    try {
        QVariant data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);
        this->m_nodeModel->finishTransaction(node);

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeAdded(node);

}
void QTOZWManager_Internal::pvt_nodeRemoved(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeRemoved " << node;
    QVector<quint64> valueList(this->m_validValues);
    for (QVector<quint64>::Iterator it = valueList.begin(); it != valueList.end(); it++) {
        if (this->m_valueModel->getValueData(*it, QTOZW_ValueIds::ValueIdColumns::Node).toInt() == node) 
            this->pvt_valueRemoved(*it);
    }
    emit this->nodeRemoved(node);

    if (this->m_validNodes.contains(node))
        this->m_validNodes.removeAll(node);

    this->m_associationsModel->delNode(node);
    /* technically, this shouldn't be required... but just in case */
    this->m_valueModel->delNodeValues(node);
    this->m_nodeModel->delNode(node);

}
void QTOZWManager_Internal::pvt_nodeReset(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeReset " << node;
    QVector<quint64> valueList(this->m_validValues);
    for (QVector<quint64>::Iterator it = valueList.begin(); it != valueList.end(); it++) {
        if (this->m_valueModel->getValueData(*it, QTOZW_ValueIds::ValueIdColumns::Node).toInt() == node) 
            this->pvt_valueRemoved(*it);
    }

    emit this->nodeReset(node);

    if (this->m_validNodes.contains(node))
        this->m_validNodes.removeAll(node);

    this->m_associationsModel->delNode(node);
    /* technically, this shouldn't be required... but just in case */
    this->m_valueModel->delNodeValues(node);
    this->m_nodeModel->delNode(node);

}
void QTOZWManager_Internal::pvt_nodeNaming(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeNaming " << node;
    try {
        QString data = this->m_manager->GetNodeName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeName, data, true);
        data = this->m_manager->GetNodeLocation(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeLocation, data, true);

        data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);
        this->m_nodeModel->finishTransaction(node);

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeNaming(node);
}
void QTOZWManager_Internal::pvt_nodeEvent(quint8 node, quint8 event)
{
    qCDebug(notifications) << "Notification pvt_nodeEvent " << node << " Event: " << event;
    try {
        QVariant data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);
        this->m_nodeModel->finishTransaction(node);

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeEvent(node, event);
}
void QTOZWManager_Internal::pvt_nodeProtocolInfo(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeProtocolInfo " << node;
    try {
        QVariant data = this->m_manager->GetNodeProductName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductName, data, true);

        data = this->m_manager->GetNodeManufacturerName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerName, data, true);

        data = this->m_manager->GetNodeManufacturerId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerID, data, true);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data, true);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data, true);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data, true);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data, true);

        data = this->m_manager->GetNodeBasicString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasicString, data, true);

        data = this->m_manager->GetNodeBasic(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasic, data, true);
        
        data = this->m_manager->GetNodeGenericString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGenericString, data, true);

        data = this->m_manager->GetNodeGeneric(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGeneric, data, true);

        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecific, data, true);

        data = this->m_manager->GetNodeSpecificString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecificString, data, true);

        data = this->m_manager->GetNodeMaxBaudRate(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBaudRate, data, true);

        data = this->m_manager->GetNodeVersion(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeVersion, data, true);

        data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        /* set our Flags */
        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isListening, this->m_manager->IsNodeListeningDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFlirs, this->m_manager->IsNodeFrequentListeningDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isBeaming, this->m_manager->IsNodeBeamingDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isRouting, this->m_manager->IsNodeRoutingDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);
        this->m_nodeModel->finishTransaction(node);

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeProtocolInfo(node);
}
void QTOZWManager_Internal::pvt_nodeEssentialNodeQueriesComplete(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeEssentialNodeQueriesComplete " << node;
    try {
        QVariant data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        data = this->m_manager->GetNodeProductName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductName, data, true);

        data = this->m_manager->GetNodeManufacturerName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerName, data, true);

        data = this->m_manager->GetNodeManufacturerId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerID, data, true);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data, true);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data, true);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data, true);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data, true);

        data = this->m_manager->GetNodeBasicString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasicString, data, true);

        data = this->m_manager->GetNodeBasic(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasic, data, true);

        data = this->m_manager->GetNodeGenericString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGenericString, data, true);

        data = this->m_manager->GetNodeGeneric(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGeneric, data, true);

        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecific, data, true);

        data = this->m_manager->GetNodeSpecificString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecificString, data, true);

        data = this->m_manager->GetNodeMaxBaudRate(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBaudRate, data, true);

        data = this->m_manager->GetNodeVersion(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeVersion, data, true);

        /* set our Flags */
        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isListening, this->m_manager->IsNodeListeningDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFlirs, this->m_manager->IsNodeFrequentListeningDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isBeaming, this->m_manager->IsNodeBeamingDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isRouting, this->m_manager->IsNodeRoutingDevice(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);

        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGroups, this->m_manager->GetNumGroups(this->homeId(), node), true);

        this->m_nodeModel->finishTransaction(node);

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeEssentialNodeQueriesComplete(node);
}
void QTOZWManager_Internal::pvt_nodeQueriesComplete(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeQueriesComplete " << node;
    /* Plus Type Info here */
    try {
        QVariant data = this->m_manager->GetNodeDeviceType(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeDeviceType, data, true);

        data = this->m_manager->GetNodeDeviceTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeDeviceTypeString, data, true);

        data = this->m_manager->GetNodeRole(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeRole, data, true);

        data = this->m_manager->GetNodeRoleString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeRoleString, data, true);

        data = this->m_manager->GetNodePlusType(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodePlusType, data, true);

        data = this->m_manager->GetNodePlusTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodePlusTypeString, data, true);

        data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data, true);

        data = this->m_manager->GetNodeProductName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductName, data, true);

        data = this->m_manager->GetNodeManufacturerName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerName, data, true);

        data = this->m_manager->GetNodeManufacturerId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerID, data, true);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data, true);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data, true);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data, true);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data, true);

        data = this->m_manager->GetNodeBasicString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasicString, data, true);

        data = this->m_manager->GetNodeBasic(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasic, data, true);

        data = this->m_manager->GetNodeGenericString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGenericString, data, true);

        data = this->m_manager->GetNodeGeneric(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGeneric, data, true);

        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecific, data, true);

        data = this->m_manager->GetNodeSpecificString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecificString, data, true);

        data = this->m_manager->GetNodeMaxBaudRate(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBaudRate, data, true);

        data = this->m_manager->GetNodeVersion(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeVersion, data, true);


        /* set our Flags */
        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isZWavePlus, this->m_manager->IsNodeZWavePlus(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node), true);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node), true);

        this->m_nodeModel->finishTransaction(node);

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeQueriesComplete(node);
}
void QTOZWManager_Internal::pvt_driverReady(quint32 _homeID)
{
    qCDebug(notifications) << "Notification pvt_driverRead " << _homeID;
    this->setHomeId(_homeID);
    emit this->started(_homeID);
    emit this->driverReady(_homeID);
}
void QTOZWManager_Internal::pvt_driverFailed(quint32 _homeID)
{
    qCDebug(notifications) << "Notification pvt_driverFailed " << _homeID;
    QVector<quint8> nodeList(this->m_validNodes);
    for (QVector<quint8>::iterator it = nodeList.begin(); it != nodeList.end(); it++) 
        this->pvt_nodeRemoved(*it);

    this->m_associationsModel->resetModel();
    this->m_valueModel->resetModel();
    this->m_nodeModel->resetModel();
    emit this->stopped(this->homeId());
    emit this->driverFailed(_homeID);
    this->setHomeId(0);
}
void QTOZWManager_Internal::pvt_driverReset(quint32 _homeID)
{
    qCDebug(notifications) << "Notification pvt_driverReset " << _homeID;
    QVector<quint8> nodeList(this->m_validNodes);
    for (QVector<quint8>::iterator it = nodeList.begin(); it != nodeList.end(); it++) 
        this->pvt_nodeRemoved(*it);

    this->m_associationsModel->resetModel();
    this->m_valueModel->resetModel();
    this->m_nodeModel->resetModel();
    emit this->stopped(this->homeId());
    emit this->driverReset(_homeID);
    this->setHomeId(0);
}
void QTOZWManager_Internal::pvt_driverRemoved(quint32 _homeID)
{
    qCDebug(notifications) << "Notification pvt_driverRemoved " << _homeID;
    QVector<quint8> nodeList(this->m_validNodes);
    for (QVector<quint8>::iterator it = nodeList.begin(); it != nodeList.end(); it++) 
        this->pvt_nodeRemoved(*it);

    this->m_associationsModel->resetModel();
    this->m_valueModel->resetModel();
    this->m_nodeModel->resetModel();
    emit this->stopped(this->homeId());
    emit this->driverRemoved(_homeID);
    this->setHomeId(0);
}
void QTOZWManager_Internal::pvt_driverAllNodesQueriedSomeDead()
{
    qCDebug(notifications) << "Notification pvt_driverAllNodesQueriedSomeDead";
    emit this->driverAllNodesQueriedSomeDead();
}
void QTOZWManager_Internal::pvt_driverAllNodesQueried()
{
    qCDebug(notifications) << "Notification pvt_driverAllNodesQueried";
    emit this->driverAllNodesQueried();
}
void QTOZWManager_Internal::pvt_driverAwakeNodesQueried()
{
    qCDebug(notifications) << "Notification pvt_driverAwakeNodesQueried";
    emit this->driverAllNodesQueried();
}
void QTOZWManager_Internal::pvt_controllerCommand(quint8 node, quint32 cmd, quint32 state, quint32 error)
{
    NotificationTypes::QTOZW_Notification_Controller_Cmd qtozwcmd = static_cast<NotificationTypes::QTOZW_Notification_Controller_Cmd>(cmd);
    NotificationTypes::QTOZW_Notification_Controller_State qtozwstate = static_cast<NotificationTypes::QTOZW_Notification_Controller_State>(state);
    NotificationTypes::QTOZW_Notification_Controller_Error qtozwerror = static_cast<NotificationTypes::QTOZW_Notification_Controller_Error>(error);
    qCDebug(notifications) << "Notification pvt_controllerCommand " << cmd << state << error;
    emit this->controllerCommand(node, qtozwcmd, qtozwstate, qtozwerror );
}
void QTOZWManager_Internal::pvt_ozwNotification(quint8 node, quint32 event)
{
    NotificationTypes::QTOZW_Notification_Code qtozwevent = static_cast<NotificationTypes::QTOZW_Notification_Code>(event);
    qCDebug(notifications) << "Notification pvt_ozwNotification" << qtozwevent;
    emit this->ozwNotification(node, qtozwevent);
}
void QTOZWManager_Internal::pvt_ozwUserAlert(quint8 node, quint32 event, quint8 retry)
{
    NotificationTypes::QTOZW_Notification_User qtozwuser = static_cast<NotificationTypes::QTOZW_Notification_User>(event);
    qCDebug(notifications) << "Notification pvt_ozwUserAlert"  << qtozwuser;
    emit this->ozwUserAlert(node, qtozwuser, retry);

}
void QTOZWManager_Internal::pvt_manufacturerSpecificDBReady()
{
    qCDebug(notifications) << "Notification pvt_manufacturerSpecificDBReady";
    emit this->manufacturerSpecificDBReady();
}

void QTOZWManager_Internal::pvt_nodeModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(bottomRight);
    if (!roles.contains(QTOZW_UserRoles::ModelDataChanged)) {
        return;
    }
    qCDebug(nodeModel) << "nodeModel Changed!" << static_cast<QTOZW_Nodes::NodeColumns>(topLeft.column()) << ": "<< topLeft.data();
    /* get the Node Number */
    QModelIndex nodeIdIndex = topLeft.siblingAtColumn(QTOZW_Nodes::NodeColumns::NodeID);
    quint8 nodeId = this->m_nodeModel->data(nodeIdIndex, Qt::DisplayRole).value<quint8>();
    try {
        switch (static_cast<QTOZW_Nodes::NodeColumns>(topLeft.column())) {
            case QTOZW_Nodes::NodeColumns::NodeName:
                this->m_manager->SetNodeName(this->homeId(), nodeId, topLeft.data().toString().toStdString());
                break;
            case QTOZW_Nodes::NodeColumns::NodeLocation:
                this->m_manager->SetNodeLocation(this->homeId(), nodeId, topLeft.data().toString().toStdString());
                break;
            default:
                qCWarning(nodeModel) << "Got a nodeModelDataChanged Singal but not a Column we handle: " << static_cast<QTOZW_Nodes::NodeColumns>(topLeft.column()) ;
                return;
        }
    } catch (OpenZWave::OZWException &e) {
        qCWarning(nodeModel) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }

}

void QTOZWManager_Internal::pvt_valueModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
    Q_UNUSED(bottomRight);
    if (!roles.contains(QTOZW_UserRoles::ModelDataChanged)) {
        return;
    }
    qCDebug(valueModel) << "valueModel Changed!" << static_cast<QTOZW_ValueIds::ValueIdColumns>(topLeft.column()) << ": "<< topLeft.data();
    /* get the Node Number */
    quint64 vidKey = this->m_valueModel->data(topLeft.siblingAtColumn(QTOZW_ValueIds::ValueIdColumns::ValueIDKey), Qt::DisplayRole).value<quint64>();
    try {
        OpenZWave::ValueID vid(this->homeId(), vidKey);
        switch (static_cast<OpenZWave::ValueID::ValueType>(vid.GetType())) {
            case OpenZWave::ValueID::ValueType_Bool:
            {
                this->m_manager->SetValue(vid, topLeft.data().toBool());
                return;
            }
            case OpenZWave::ValueID::ValueType_Byte:
            {
                this->m_manager->SetValue(vid, topLeft.data().value<quint8>());
                return;
            }
            case OpenZWave::ValueID::ValueType_Decimal:
            {
                this->m_manager->SetValue(vid, topLeft.data().toFloat());
                return;
            }
            case OpenZWave::ValueID::ValueType_Int:
            {
                this->m_manager->SetValue(vid, topLeft.data().value<int32>());
                return;
            }
            case OpenZWave::ValueID::ValueType_List:
            {
                QTOZW_ValueIDList list = topLeft.data().value<QTOZW_ValueIDList>();
                this->m_manager->SetValueListSelection(vid, list.selectedItem.toStdString().c_str());
                return;
            }
            case OpenZWave::ValueID::ValueType_Schedule:
            {
                qCWarning(valueModel) << "ValueType_Schedule TODO";
                return;
            }
            case OpenZWave::ValueID::ValueType_Short:
            {
                this->m_manager->SetValue(vid, topLeft.data().value<int16>());
                return;
            }
            case OpenZWave::ValueID::ValueType_String:
            {
                this->m_manager->SetValue(vid, topLeft.data().toString().toStdString());
                return;
            }
            case OpenZWave::ValueID::ValueType_Button:
            {
                qCWarning(valueModel) << "ValueType_Button TODO";
                return;
            }
            case OpenZWave::ValueID::ValueType_Raw:
            {
                qCWarning(valueModel) << "ValueType_Raw TODO";
                return;
            }
            case OpenZWave::ValueID::ValueType_BitSet:
            {
                QTOZW_ValueIDBitSet bs = topLeft.data().value<QTOZW_ValueIDBitSet>();
                for (int i = 0; i <= bs.values.size()-1; i++) {
                    if (bs.mask.at(i)) {
                        bool curval;
                        this->m_manager->GetValueAsBitSet(vid, (quint8)i+1, &curval);
                        if (curval != bs.values.at(i)) {
                            /* we send this as a quint32 so its a atomic change... and return.
                             * as the chances of other Bits changing are probably high, and a
                             * each call to SetValue generates traffis, so instead by sending a
                             * quint32 here, we only send one packet*/
                            this->m_manager->SetValue(vid, (qint32)BitSettoInteger(bs.values));
                            return;
                        }
                    }
                }
                return;
            }

        }
    } catch (OpenZWave::OZWException &e) {
        qCWarning(valueModel) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWManagerErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
}
