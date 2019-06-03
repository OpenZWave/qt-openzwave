#include "qtozwmanager_p.h"
#include "qt-openzwave/qtozwmanager.h"

/* XXX TODO: this needs to go into OZW */
QString nodeBasicStr (uint8 basic)
{
  switch (basic) {
  case 1:
    return "Controller";
  case 2:
    return "Static Controller";
  case 3:
    return "Slave";
  case 4:
    return "Routing Slave";
  }
  return "unknown";
}


QTOZWManager_Internal::QTOZWManager_Internal(QObject *parent)
    : QTOZWManagerSimpleSource (parent)
{
    this->setObjectName("QTOZW_Manager");
    try {
        this->m_options = OpenZWave::Options::Create( "", "", "" );
    } catch (OpenZWave::OZWException &e) {
        qCWarning(manager) << "Failed to Load Options Class" << QString(e.GetMsg().c_str());
        return;
    }
    qCDebug(manager) << "OpenZWave Options Class Created";
    this->m_options->AddOptionInt( "SaveLogLevel", OpenZWave::LogLevel_Detail );
    this->m_options->AddOptionInt( "QueueLogLevel", OpenZWave::LogLevel_Debug );
    this->m_options->AddOptionInt( "DumpTrigger", OpenZWave::LogLevel_Error );
    this->m_options->AddOptionBool("ConsoleOutput", false);
    this->m_options->AddOptionInt( "PollInterval", 500 );
    this->m_options->AddOptionBool( "IntervalBetweenPolls", true );
    this->m_options->AddOptionBool( "ValidateValueChanges", true);
    qCDebug(manager) << "OpenZWave Options Set";


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
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Load Manager Class" << QString(e.GetMsg().c_str());
        return false;
    }
    qCDebug(manager) << "OpenZWave Manager Instance Created";
    try {
        if (this->m_manager->AddWatcher( OZWNotification::processNotification, this ) != true) {
            emit this->error(QTOZWErrorCodes::setupFailed);
            this->setErrorString("Failed to Add Notification Callback");
            qCWarning(manager) << "Failed to Add Notification Callback";
            return false;
        }
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWErrorCodes::OZWException);
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
            emit this->error(QTOZWErrorCodes::setupFailed);
            this->setErrorString("Failed to Add Serial Port");
            qCWarning(manager) << "Failed to Add Serial Port";
            return false;
        }
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
        qCWarning(manager) << "Failed to Add Serial Port: " << QString(e.GetMsg().c_str());
        return false;
    }
    qCDebug(manager) << "AddDriver Completed";

    return true;
}

bool QTOZWManager_Internal::refreshNodeInfo(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->RefreshNodeInfo(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
}
bool QTOZWManager_Internal::sendNodeInfomation(quint8 _node) {
    if (!this->checkHomeId() || !this->checkNodeId(_node))
        return false;
    try {
        return this->m_manager->SendNodeInformation(this->homeId(), _node);
    } catch (OpenZWave::OZWException &e) {
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
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
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    return false;
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

bool QTOZWManager_Internal::checkHomeId() {
    if (this->m_manager == nullptr) {
        emit this->error(QTOZWErrorCodes::Manager_Not_Started);
        this->setErrorString("Manager Not Started");
        return false;
    }
    if (this->homeId() == 0) {
        emit this->error(QTOZWErrorCodes::homeId_Invalid);
        this->setErrorString("Invalid homeID");
        return false;
    }
    return true;
}
bool QTOZWManager_Internal::checkNodeId(quint8 _node) {
    if (!this->m_validNodes.contains(_node)) {
        emit this->error(QTOZWErrorCodes::nodeId_Invalid);
        this->setErrorString("Invalid nodeID");
        return false;
    }
    return true;
}

bool QTOZWManager_Internal::checkValueKey(quint64 _vidKey) {
    if (!this->m_validValues.contains(_vidKey)) {
        emit this->error(QTOZWErrorCodes::valueIDKey_Invalid);
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
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(value));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Bool);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Byte:
        {
            quint8 value;
            this->m_manager->GetValueAsByte(vid, &value);
            /* QT has a habbit of treating quint8 as char... so cast it to 32 to get around that */
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(static_cast<quint32>(value)));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Byte);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Decimal:
        {
            std::string value;
            this->m_manager->GetValueAsString(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(QString(value.c_str()).toFloat()));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Decimal);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Int:
        {
            qint32 value;
            this->m_manager->GetValueAsInt(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(value));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Int);
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
                    vidlist.values.push_back(values.at(i));
                    i++;
                }
                std::string selectedItem;
                this->m_manager->GetValueListSelection(vid, &selectedItem);
                vidlist.selectedItem = QString::fromStdString(selectedItem);
                this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(vidlist));
            }
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::List);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Schedule:
        {
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Schedule);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Short:
        {
            int16_t value;
            this->m_manager->GetValueAsShort(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(value));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Short);
            return true;
        }
        case OpenZWave::ValueID::ValueType_String:
        {
            std::string value;
            this->m_manager->GetValueAsString(vid, &value);
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(QString(value.c_str())));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::String);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Button:
        {
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Button);
            return true;
        }
        case OpenZWave::ValueID::ValueType_Raw:
        {
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::Raw);
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
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value, QVariant::fromValue(vidbs));
            this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type, QTOZW_ValueIds::ValueIdTypes::BitSet);
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

    try {
        OpenZWave::ValueID vid(this->homeId(), vidKey);
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Label, this->m_manager->GetValueLabel(vid).c_str());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Instance, vid.GetInstance());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::CommandClass, vid.GetCommandClassId());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Index, vid.GetIndex());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node, vid.GetNodeId());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Genre, vid.GetGenre());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Help, this->m_manager->GetValueHelp(vid).c_str());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Units, this->m_manager->GetValueUnits(vid).c_str());
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Min, this->m_manager->GetValueMin(vid));
        this->m_valueModel->setValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Max, this->m_manager->GetValueMax(vid));
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ReadOnly, this->m_manager->IsValueReadOnly(vid));
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::WriteOnly, this->m_manager->IsValueWriteOnly(vid));
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValueSet, this->m_manager->IsValueSet(vid));
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValuePolled, this->m_manager->IsValuePolled(vid));
        this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ChangeVerified, this->m_manager->GetChangeVerified(vid));

        this->convertValueID(vidKey);
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->valueAdded(vidKey);
}
void QTOZWManager_Internal::pvt_valueRemoved(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valueRemoved: " << vidKey;
    if (this->m_validValues.contains(vidKey))
        this->m_validValues.removeAll(vidKey);
    this->m_valueModel->delValue(vidKey);
    emit this->valueRemoved(vidKey);
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
    this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValuePolled, true);

}
void QTOZWManager_Internal::pvt_valuePollingDisabled(quint64 vidKey)
{
    qCDebug(notifications) << "Notification pvt_valuePollingDisabled " << vidKey;
    this->m_valueModel->setValueFlags(vidKey, QTOZW_ValueIds::ValueIDFlags::ValuePolled, false);

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
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
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
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeAdded(node);

}
void QTOZWManager_Internal::pvt_nodeRemoved(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeRemoved " << node;
    if (this->m_validNodes.contains(node))
        this->m_validNodes.removeAll(node);
    this->m_associationsModel->delNode(node);
    /* technically, this shouldn't be required... but just in case */
    this->m_valueModel->delNodeValues(node);
    this->m_nodeModel->delNode(node);
    emit this->nodeRemoved(node);

}
void QTOZWManager_Internal::pvt_nodeReset(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeReset " << node;
    if (this->m_validNodes.contains(node))
        this->m_validNodes.removeAll(node);

    this->m_associationsModel->delNode(node);
    /* technically, this shouldn't be required... but just in case */
    this->m_valueModel->delNodeValues(node);
    this->m_nodeModel->delNode(node);
    emit this->nodeReset(node);

}
void QTOZWManager_Internal::pvt_nodeNaming(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeNaming " << node;
    try {
        QString data = this->m_manager->GetNodeName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeName, data);
        data = this->m_manager->GetNodeLocation(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeLocation, data);

        data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeNaming(node);
}
void QTOZWManager_Internal::pvt_nodeEvent(quint8 node, quint8 event)
{
    qCDebug(notifications) << "Notification pvt_nodeEvent " << node << " Event: " << event;
    try {
        QVariant data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeEvent(node, event);
}
void QTOZWManager_Internal::pvt_nodeProtocolInfo(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeProtocolInfo " << node;
    try {
        QVariant data = this->m_manager->GetNodeProductName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductName, data);

        data = this->m_manager->GetNodeManufacturerName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerName, data);

        data = this->m_manager->GetNodeManufacturerId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerID, data);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data);

        data = nodeBasicStr(this->m_manager->GetNodeBasic(this->homeId(), node));
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasicString, data);

        data = this->m_manager->GetNodeBasic(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasic, data);

        data = this->m_manager->GetNodeDeviceTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGenericString, data);

        data = this->m_manager->GetNodeGeneric(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGeneric, data);

        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecific, data);

        /* XXX TODO: Need a OZW SpecificString function */
        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecificString, data);

        data = this->m_manager->GetNodeMaxBaudRate(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBaudRate, data);

        data = this->m_manager->GetNodeVersion(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeVersion, data);

        data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        /* set our Flags */
        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isListening, this->m_manager->IsNodeListeningDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFlirs, this->m_manager->IsNodeFrequentListeningDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isBeaming, this->m_manager->IsNodeBeamingDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isRouting, this->m_manager->IsNodeRoutingDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));
    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
    emit this->nodeProtocolInfo(node);
}
void QTOZWManager_Internal::pvt_nodeEssentialNodeQueriesComplete(quint8 node)
{
    qCDebug(notifications) << "Notification pvt_nodeEssentialNodeQueriesComplete " << node;
    try {
        QVariant data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        data = this->m_manager->GetNodeProductName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductName, data);

        data = this->m_manager->GetNodeManufacturerName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerName, data);

        data = this->m_manager->GetNodeManufacturerId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerID, data);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data);

        data = nodeBasicStr(this->m_manager->GetNodeBasic(this->homeId(), node));
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasicString, data);

        data = this->m_manager->GetNodeBasic(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasic, data);

        data = this->m_manager->GetNodeDeviceTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGenericString, data);

        data = this->m_manager->GetNodeGeneric(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGeneric, data);

        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecific, data);

        /* XXX TODO: Need a OZW SpecificString function */
        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecificString, data);

        data = this->m_manager->GetNodeMaxBaudRate(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBaudRate, data);

        data = this->m_manager->GetNodeVersion(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeVersion, data);

        /* set our Flags */
        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isListening, this->m_manager->IsNodeListeningDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFlirs, this->m_manager->IsNodeFrequentListeningDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isBeaming, this->m_manager->IsNodeBeamingDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isRouting, this->m_manager->IsNodeRoutingDevice(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));

        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGroups, this->m_manager->GetNumGroups(this->homeId(), node));

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
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
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeDeviceType, data);

        data = this->m_manager->GetNodeDeviceTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeDeviceTypeString, data);

        data = this->m_manager->GetNodeRole(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeRole, data);

        data = this->m_manager->GetNodeRoleString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeRoleString, data);

        data = this->m_manager->GetNodePlusType(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodePlusType, data);

        data = this->m_manager->GetNodePlusTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodePlusTypeString, data);

        data = this->m_manager->GetNodeQueryStage(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeQueryStage, data);

        data = this->m_manager->GetNodeProductName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductName, data);

        data = this->m_manager->GetNodeManufacturerName(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerName, data);

        data = this->m_manager->GetNodeManufacturerId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeManufacturerID, data);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data);

        data = this->m_manager->GetNodeProductId(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductID, data);

        data = this->m_manager->GetNodeProductType(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeProductType, data);

        data = nodeBasicStr(this->m_manager->GetNodeBasic(this->homeId(), node));
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasicString, data);

        data = this->m_manager->GetNodeBasic(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBasic, data);

        data = this->m_manager->GetNodeDeviceTypeString(this->homeId(), node).c_str();
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGenericString, data);

        data = this->m_manager->GetNodeGeneric(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeGeneric, data);

        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecific, data);

        /* XXX TODO: Need a OZW SpecificString function */
        data = this->m_manager->GetNodeSpecific(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeSpecificString, data);

        data = this->m_manager->GetNodeMaxBaudRate(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeBaudRate, data);

        data = this->m_manager->GetNodeVersion(this->homeId(), node);
        this->m_nodeModel->setNodeData(node, QTOZW_Nodes::NodeVersion, data);


        /* set our Flags */
        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isZWavePlus, this->m_manager->IsNodeZWavePlus(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isNIFRecieved, this->m_manager->IsNodeInfoReceived(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isAwake, this->m_manager->IsNodeAwake(this->homeId(), node));

        this->m_nodeModel->setNodeFlags(node, QTOZW_Nodes::isFailed, this->m_manager->IsNodeFailed(this->homeId(), node));

    } catch (OpenZWave::OZWException &e) {
        qCWarning(notifications) << "OZW Exception: " << e.GetMsg().c_str() << " at " << e.GetFile().c_str() <<":" << e.GetLine();
        emit this->error(QTOZWErrorCodes::OZWException);
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
void QTOZWManager_Internal::pvt_controllerCommand(quint8 command)
{
    qCDebug(notifications) << "Notification pvt_controllerCommand " << command;
    emit this->controllerCommand(command);
}
void QTOZWManager_Internal::pvt_ozwNotification(OpenZWave::Notification::NotificationCode event)
{
    qCDebug(notifications) << "Notification pvt_ozwNotification" << event;

}
void QTOZWManager_Internal::pvt_ozwUserAlert(OpenZWave::Notification::UserAlertNotification event)
{
    qCDebug(notifications) << "Notification pvt_ozwUserAlert"  << event;

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
        emit this->error(QTOZWErrorCodes::OZWException);
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
                qCWarning(valueModel) << "ValueType List TODO";
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
        emit this->error(QTOZWErrorCodes::OZWException);
        this->setErrorString(e.GetMsg().c_str());
    }
}
