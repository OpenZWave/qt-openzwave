#include <QDateTime>

#include "mqttpublisher.h"
#include "qtrj.h"
#include "mqttcommands/mqttcommands.h"

#include <rapidjson/prettywriter.h> // for stringify JSON


Q_LOGGING_CATEGORY(ozwmp, "ozw.mqtt.publisher");
Q_LOGGING_CATEGORY(ozwmpnode, "ozw.mqtt.publisher.node");
Q_LOGGING_CATEGORY(ozwmpvalue, "ozw.mqtt.publisher.value");



mqttNodeModel::mqttNodeModel(QObject *parent)
{

}

QVariant mqttNodeModel::getNodeData(quint8 node, NodeColumns col) {
    int row = this->getNodeRow(node);
    return this->data(this->index(row, col), Qt::DisplayRole);
}

bool mqttNodeModel::isValidNode(quint8 node) {
    if (this->getNodeRow(node) == -1)
        return false;
    return true;
}


bool mqttNodeModel::populateJsonObject(rapidjson::Document &jsonobject, quint8 node, QTOZWManager *mgr) {
    if (jsonobject.IsNull())
        jsonobject.SetObject();

    for (int i = 0; i < this->columnCount(QModelIndex()); i++) {
        QVariant data = this->getNodeData(node, static_cast<NodeColumns>(i));
        if (data.type() == QVariant::Invalid) {
            continue;
        }
        switch (static_cast<NodeColumns>(i)) {
            case NodeColumns::NodeFlags: {
                QBitArray flag = data.toBitArray();
                QMetaEnum metaEnum = QMetaEnum::fromType<nodeFlags>();
                for (int j = 0; j < nodeFlags::flagCount; j++) {
                    QT2JS::SetBool(jsonobject, metaEnum.valueToKey(j), flag.at(j));
                }
                break;
            }
            default: {
                QMetaEnum metaEnum = QMetaEnum::fromType<NodeColumns>();
                if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QString) {
                    QT2JS::SetString(jsonobject, metaEnum.valueToKey(i), data.toString());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Bool) {
                    QT2JS::SetBool(jsonobject, metaEnum.valueToKey(i), data.toBool());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Int) {
                    QT2JS::SetInt(jsonobject, metaEnum.valueToKey(i), data.toInt());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::UInt) {
                    QT2JS::SetUint(jsonobject, metaEnum.valueToKey(i), data.toUInt());
                } else {
                    qCWarning(ozwmpvalue) << "Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << node;
                }
                break;
            }
        }

    }
    /* MetaData */
    rapidjson::Value metadata;
    if (!jsonobject.HasMember("MetaData")) {
        rapidjson::Value metadata;
        metadata.SetObject();
        QMetaEnum metaEnum = QMetaEnum::fromType<QTOZWManagerSource::QTOZWMetaDataField>();
        if (metadata.IsNull()) {
            metadata.SetObject();
        }
        for (int i = 0; i < QTOZWManagerSource::Identifier; i++) {
            metadata.AddMember(rapidjson::Value(metaEnum.valueToKey(i), jsonobject.GetAllocator()).Move(),
                    rapidjson::Value(mgr->GetMetaData(node, static_cast<QTOZWManagerSource::QTOZWMetaDataField>(i)).toStdString().c_str(), jsonobject.GetAllocator()).Move(),
                    jsonobject.GetAllocator());
        }
        metadata.AddMember(rapidjson::Value("ProductPicBase64").Move(), 
            rapidjson::Value(QString(mgr->GetMetaDataProductPic(node).toBase64()).toStdString().c_str(), jsonobject.GetAllocator()).Move(),
            jsonobject.GetAllocator());

        jsonobject.AddMember(rapidjson::Value("MetaData"), metadata, jsonobject.GetAllocator());
    }
    /* Neighbors */
    QVector<quint8> neighbors = mgr->GetNodeNeighbors(node);
    if (neighbors.size() > 0) {
        rapidjson::Value N(rapidjson::kArrayType);
        for (int i = 0; i < neighbors.count(); i++) {
            N.PushBack(neighbors[i], jsonobject.GetAllocator());
        }
        jsonobject.AddMember(rapidjson::Value("Neighbors").Move(), N, jsonobject.GetAllocator());
    }
    return true;
}

mqttValueIDModel::mqttValueIDModel(QObject *parent) {

}
QVariant mqttValueIDModel::getValueData(quint64 vidKey, ValueIdColumns col) {
    int row = this->getValueRow(vidKey);
    return this->data(this->index(row, col), Qt::DisplayRole);
}

bool mqttValueIDModel::isValidValueID(quint64 vidKey) {
    if (this->getValueRow(vidKey) == -1) 
        return false;
    return true;
}


bool mqttValueIDModel::populateJsonObject(QJsonObject *jsonobject, quint64 vidKey, QTOZWManager *mgr) {
    for (int i = 0; i < ValueIdColumns::ValueIdCount; i++) {
        QVariant data = this->getValueData(vidKey, static_cast<ValueIdColumns>(i));
        switch (static_cast<ValueIdColumns>(i)) {
        case ValueFlags: {
            QBitArray flag = data.toBitArray();
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIDFlags>();
            for (int j = 0; j < ValueIDFlags::FlagCount; j++) {
                jsonobject->insert(metaEnum.valueToKey(j), flag.at(j));
            }
            break;
        }
        case Value: {
            jsonobject->insert("Value", this->encodeValue(vidKey));
            break;
        }
        case Genre: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdGenres>();
            jsonobject->insert("Genre", metaEnum.valueToKey(data.toInt()));
            break;
        }
        case Type: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdTypes>();
            jsonobject->insert("Type", metaEnum.valueToKey(data.toInt()));
            break;
        }
        case CommandClass: {
            jsonobject->insert("CommandClass", mgr->getCommandClassString(data.toInt()));
            break;
        }

        default: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdColumns>();
            if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QString) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toString());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Bool) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toBool());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Int) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::UInt) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Float) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toDouble());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::ULongLong) {
                jsonobject->insert(metaEnum.valueToKey(i), static_cast<qint64>(data.toULongLong()));
            } else {
                qCWarning(ozwmpvalue) << "mqttValueIDModel::populateJsonObject: Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << vidKey;
            }
            break;
        }

        }
    }
    return true;
}

QJsonValue mqttValueIDModel::encodeValue(quint64 vidKey) {
    QJsonValue value;
    QVariant data = this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Value);
    QTOZW_ValueIds::ValueIdTypes type = this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Type).value<QTOZW_ValueIds::ValueIdTypes>();
    switch (type) {
        case QTOZW_ValueIds::ValueIdTypes::BitSet: {
            QJsonArray bitsets;
            QTOZW_ValueIDBitSet vidbs = data.value<QTOZW_ValueIDBitSet>();
            int size = vidbs.mask.size();
            for (int i = 0; i < size; i++) {
                if (vidbs.mask[i] == 1) {
                    QJsonObject bitset;
                    bitset["Label"] = vidbs.label[i];
                    bitset["Help"] = vidbs.help[i];
                    bitset["Value"] = static_cast<bool>(vidbs.values[i]);
                    bitset["Position"] = i;
                    bitsets.push_back(bitset);
                }
            }
            value = bitsets;
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            value = data.toBool();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Button: {
            value = data.toBool();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            value = data.toInt();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Decimal: {
            value = data.toFloat();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:{
            value = data.toInt();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::List: {
            QTOZW_ValueIDList vidlist = data.value<QTOZW_ValueIDList>();
            int size = vidlist.values.count();
            QJsonArray list;
            for (int i = 0; i < size; i++) {
                QJsonObject entry;
                entry["Value"] = static_cast<int>(vidlist.values[i]);
                entry["Label"] = vidlist.labels[i];
                list.push_back(entry);
            }
            QJsonObject var;
            var["List"] = list;
            var["Selected"] = vidlist.selectedItem;
            value = var;
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Raw: {
            qCWarning(ozwmpvalue) << "Raw ValueType not handled in mqttValueIdModel::encodeValue yet";
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Schedule: {
            qCWarning(ozwmpvalue) << "Raw ValueType not handled in mqttValueIdModel::encodeValue yet";
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            value = data.toInt();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::String: {
            value = data.toString();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::TypeCount: {
            qCWarning(ozwmpvalue) << "Unhandled ValueID Type" << type << "in mqttValueIdModel::encodeValue" << vidKey << this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Label).toString();
            break;
        }
    }
    return value;
}

bool mqttValueIDModel::setData(quint64 vidKey, QVariant data) {
    return QTOZW_ValueIds::setData(this->index(this->getValueRow(vidKey), QTOZW_ValueIds::ValueIdColumns::Value), data, Qt::EditRole);
}



mqttpublisher::mqttpublisher(QSettings *settings, QObject *parent) : QObject(parent)
{
    this->settings = settings;
    this->m_client = new QMqttClient(this);
    this->m_client->setHostname(settings->value("MQTTServer", "127.0.0.1").toString());
    this->m_client->setPort(static_cast<quint16>(settings->value("MQTTPort", 1883).toInt()));

    this->m_ozwstatus.SetObject();

    /* setup the Commands */
    this->m_commands =  new MqttCommands(this);
    this->m_commands->setupCommands();


    connect(this->m_client, &QMqttClient::stateChanged, this, &mqttpublisher::updateLogStateChange);
    connect(this->m_client, &QMqttClient::disconnected, this, &mqttpublisher::brokerDisconnected);
    connect(this->m_client, &QMqttClient::errorChanged, this, &mqttpublisher::brokerError);

    connect(this->m_client, &QMqttClient::messageReceived, this, &mqttpublisher::handleMessage);
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
        qCDebug(ozwmp) << content;
    });

    this->m_client->setWillTopic(getTopic(MQTT_OZW_STATUS_TOPIC));
    QJsonObject willMsg;
    willMsg["Status"] = "Offline";
    this->m_client->setWillMessage(QJsonDocument(willMsg).toJson());
    this->m_client->setWillRetain(true);
    connect(&this->m_statsTimer, &QTimer::timeout, this, &mqttpublisher::doStats);
}

void mqttpublisher::cleanTopics(QMqttMessage msg) {
    if (msg.retain() == true) { 
        qCDebug(ozwmp) << "Topics: " << msg.topic().name();
        QJsonDocument jsmsg = QJsonDocument::fromJson(msg.payload());
        if (msg.topic().name() == getTopic("status")) {
            qCDebug(ozwmp) << jsmsg.toJson();
            /* when our status message is anything other than Offline, drop the Subscription */
            if (jsmsg["Status"].toString() != "Offline") {
                qCDebug(ozwmp) << "Unsubscribing from Topic Cleanup";
                this->m_cleanTopicSubscription->unsubscribe();
            }
            return;
        }
        if (!jsmsg["TimeStamp"].isUndefined()) {
            QDateTime ts = QDateTime::fromSecsSinceEpoch(jsmsg["TimeStamp"].toInt());
            if (ts < this->m_currentStartTime) {
                qCDebug(ozwmp) << "Removing Stale Topic/Msg: " << msg.topic().name();
                this->m_client->publish(msg.topic(), "", 0, true);
            }
        } else {
            qCWarning(ozwmp) << "MQTT Message on Topic " << msg.topic().name() << "Does not have TimeStamp - Not Cleaning: " << jsmsg.toJson();
        }
    }
}

void mqttpublisher::brokerError(QMqttClient::ClientError error) {
    qCWarning(ozwmp) << "Broker Error" << error;
    if (settings->value("MQTTTLS").toBool() == true) {
        qCWarning(ozwmp) << qobject_cast<QSslSocket *>(this->m_client->transport())->errorString();
    }

}



void mqttpublisher::doStats() {
    QJsonObject stats;
    if (!this->m_qtozwdeamon) {
        return; 
    }
    QTOZWManager *manager = this->m_qtozwdeamon->getManager();
    DriverStatistics ds = manager->GetDriverStatistics();
    QJsonObject dsjson;
    dsjson["SOFCnt"] = static_cast<qint64>(ds.m_SOFCnt);
    dsjson["ACKWaiting"] = static_cast<qint64>(ds.m_ACKWaiting);
    dsjson["readAborts"] = static_cast<qint64>(ds.m_readAborts);
    dsjson["badChecksum"] = static_cast<qint64>(ds.m_badChecksum);
    dsjson["readCnt"] = static_cast<qint64>(ds.m_readCnt);
    dsjson["writeCnt"] = static_cast<qint64>(ds.m_writeCnt);
    dsjson["CANCnt"] = static_cast<qint64>(ds.m_CANCnt);
    dsjson["NAKCnt"] = static_cast<qint64>(ds.m_NAKCnt);
    dsjson["ACKCnt"] = static_cast<qint64>(ds.m_ACKCnt);
    dsjson["OOFCnt"] = static_cast<qint64>(ds.m_OOFCnt);
    dsjson["dropped"] = static_cast<qint64>(ds.m_dropped);
    dsjson["retries"] = static_cast<qint64>(ds.m_retries);
    dsjson["callbacks"] = static_cast<qint64>(ds.m_callbacks);
    dsjson["badroutes"] = static_cast<qint64>(ds.m_badroutes);
    dsjson["noack"] = static_cast<qint64>(ds.m_noack);
    dsjson["netbusy"] = static_cast<qint64>(ds.m_netbusy);
    dsjson["notidle"] = static_cast<qint64>(ds.m_notidle);
    dsjson["txverified"] = static_cast<qint64>(ds.m_txverified);
    dsjson["nondelivery"] = static_cast<qint64>(ds.m_nondelivery);
    dsjson["routedbusy"] = static_cast<qint64>(ds.m_routedbusy);
    dsjson["broadcastReadCnt"] = static_cast<qint64>(ds.m_broadcastReadCnt);
    dsjson["broadcastWriteCnt"] = static_cast<qint64>(ds.m_broadcastWriteCnt);

    stats["Network"] = dsjson;
    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATS_TOPIC)), QJsonDocument(stats).toJson(), 0, false);

    for (int i = 0; i < this->m_nodeModel->rowCount(QModelIndex()); i++) {
        QJsonObject nsjson;
        int NodeID = this->m_nodeModel->data(this->m_nodeModel->index(i, mqttNodeModel::NodeColumns::NodeID), Qt::DisplayRole).toInt();
        NodeStatistics ns = manager->GetNodeStatistics(NodeID);
        nsjson["sendCount"] = static_cast<qint64>(ns.sentCount); /**< Number of Packets Sent to the Node */
        nsjson["sentFailed"] = static_cast<qint64>(ns.sentFailed); /**< Number of Packets that Failed to be acknowledged by the Node or Controller */
        nsjson["retries"] = static_cast<qint64>(ns.retries); /**< Number of times we have had to Retry sending packets to the Node */
        nsjson["receivedPackets"] = static_cast<qint64>(ns.receivedPackets); /**< Number of received Packets from the Node */
        nsjson["receivedDupPackets"] = static_cast<qint64>(ns.receivedDupPackets); /**< Number of Duplicate Packets received from the Node */
        nsjson["receivedUnsolicited"] = static_cast<qint64>(ns.receivedUnsolicited); /**< Number of Unsolicited Packets received from the Node */
        nsjson["lastSentTimeStamp"] = ns.lastSentTimeStamp.toSecsSinceEpoch(); /**< TimeStamp of the Last time we sent a packet to the Node */
        nsjson["lastRecievedTimeStamp"] = ns.lastReceivedTimeStamp.toSecsSinceEpoch(); /**< Timestamp of the last time we received a packet from the Node */
        nsjson["lastRequestRTT"] = static_cast<qint64>(ns.lastRequestRTT); /**<  Last Round-Trip Time when we made a request to the Node */
        nsjson["averageRequestRTT"] = static_cast<qint64>(ns.averageRequestRTT); /**< Average Round-Trip Time when we make requests to a Node */
        nsjson["lastResponseRTT"] = static_cast<qint64>(ns.lastResponseRTT); /**< Last Round-Trip Time when we got a Response from a Node */
        nsjson["averageResponseRTT"] = static_cast<qint64>(ns.averageResponseRTT); /**< Average Round-Trip Time when got a Response from a Node */
        nsjson["quality"] = ns.quality; /**< The Quality of the Signal from the Node, as Reported by the Controller */
        nsjson["extendedTXSupported"] = ns.extendedTXSupported; /**< If these statistics support Extended TX Reporting (Controller Dependent) */
        nsjson["txTime"] = ns.txTime; /**< The Time it took to Transmit the last packet to the Node */
        nsjson["hops"] = ns.hops; /**< The Number of hops the packet traversed to reach the node */
        nsjson["rssi_1"] = ns.rssi_1; /**< The RSSI Strength of the first hop */
        nsjson["rssi_2"] = ns.rssi_2; /**< The RSSI Strength of the second hop */
        nsjson["rssi_3"] = ns.rssi_3; /**< The RSSI Strength of the third hop */
        nsjson["rssi_4"] = ns.rssi_4; /**< The RSSI Strength of the fourth hop */
        nsjson["rssi_5"] = ns.rssi_5; /**< The RSSI Strength of the final hop */
        nsjson["route_1"] = ns.route_1; /**< The NodeId of the First Hop */
        nsjson["route_2"] = ns.route_2; /**< The NodeId of the Second Hop */
        nsjson["route_3"] = ns.route_3; /**< The NodeId of the third Hop */
        nsjson["route_4"] = ns.route_4; /**< The NodeId of the Fourth Hop */
        nsjson["ackChannel"] = ns.ackChannel; /**< The Channel that recieved the ACK From the Node */
        nsjson["lastTXChannel"] = ns.lastTXChannel; /**< The last Channel we used to communicate with the Node */
        nsjson["routeScheme"] = ns.routeScheme; /**< How the Route was calculated when we last communicated with the Node */
        nsjson["routeUsed"] = ns.routeUsed; /**< The Route last used to communicate with the Node */
        nsjson["routeSpeed"] = ns.routeSpeed; /**< The Speed that was used when we last communicated with the node */
        nsjson["routeTries"] = ns.routeTries; /**< The Number of attempts the Controller made to route the packet to the Node */
        nsjson["lastFailedLinkFrom"] = ns.lastFailedLinkFrom; /**< The Last Failed Link From */
        nsjson["lastFailedLinkTo"] = ns.lastFailedLinkTo; /**< The Last Failed Link To */
        this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_STATS_NODE_TOPIC, NodeID)), QJsonDocument(nsjson).toJson(), 0, false);
    }

}

bool mqttpublisher::isValidNode(quint8 node) {
    return this->m_nodeModel->isValidNode(node);
}
bool mqttpublisher::isValidValueID(quint64 vidKey) {
    return this->m_valueModel->isValidValueID(vidKey);
}
QVariant mqttpublisher::getValueData(quint64 vidKey, mqttValueIDModel::ValueIdColumns col) {
    return this->m_valueModel->getValueData(vidKey, col);
}

bool mqttpublisher::setValue(quint64 vidKey, QVariant data) {
    return this->m_valueModel->setData(vidKey, data);
}


QString mqttpublisher::getTopic(QString topic) {
        if (!topic.endsWith('#') && !topic.endsWith('/'))
            topic = topic.append("/");
        return QString(MQTT_OZW_TOP_TOPIC).arg(settings->value("Instance", 1).toInt()).append(topic);
}

QString mqttpublisher::getNodeTopic(QString topic, quint8 node) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)));
    return t;
}
QString mqttpublisher::getValueTopic(QString topic, quint8 node, quint64 vid) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)).arg(static_cast<quint64>(vid)));
    return t;
}

QString mqttpublisher::getCommandTopic() {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(QString(MQTT_OZW_COMMAND_TOPIC));
    return t;
}

QString mqttpublisher::getCommandResponseTopic(QString cmd) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(QString(MQTT_OZW_RESPONSE_TOPIC).arg(cmd));
    return t;
}

void mqttpublisher::setOZWDaemon(qtozwdaemon *ozwdaemon) {
    this->m_qtozwdeamon = ozwdaemon;

    QTOZWManager *manager = this->m_qtozwdeamon->getManager();

    this->m_nodeModel = static_cast<mqttNodeModel *>(manager->getNodeModel());
    this->m_valueModel = static_cast<mqttValueIDModel *>(manager->getValueModel());

    connect(manager, &QTOZWManager::ready, this, &mqttpublisher::ready);
    connect(manager, &QTOZWManager::valueAdded, this, &mqttpublisher::valueAdded);
    connect(manager, &QTOZWManager::valueChanged, this, &mqttpublisher::valueChanged);
    connect(manager, &QTOZWManager::valueRemoved, this, &mqttpublisher::valueRemoved);
    connect(manager, &QTOZWManager::valueRefreshed, this, &mqttpublisher::valueRefreshed);
    connect(manager, &QTOZWManager::nodeNew, this, &mqttpublisher::nodeNew);
    connect(manager, &QTOZWManager::nodeAdded, this, &mqttpublisher::nodeAdded);
    connect(manager, &QTOZWManager::nodeEvent, this, &mqttpublisher::nodeEvent);
    connect(manager, &QTOZWManager::nodeReset, this, &mqttpublisher::nodeReset);
    connect(manager, &QTOZWManager::nodeNaming, this, &mqttpublisher::nodeNaming);
    connect(manager, &QTOZWManager::nodeRemoved, this, &mqttpublisher::nodeRemoved);
    connect(manager, &QTOZWManager::nodeProtocolInfo, this, &mqttpublisher::nodeProtocolInfo);
    connect(manager, &QTOZWManager::nodeEssentialNodeQueriesComplete, this, &mqttpublisher::nodeEssentialNodeQueriesComplete);
    connect(manager, &QTOZWManager::nodeQueriesComplete, this, &mqttpublisher::nodeQueriesComplete);
    connect(manager, &QTOZWManager::driverReady, this, &mqttpublisher::driverReady);
    connect(manager, &QTOZWManager::driverReset, this, &mqttpublisher::driverReset);
    connect(manager, &QTOZWManager::driverFailed, this, &mqttpublisher::driverFailed);
    connect(manager, &QTOZWManager::driverRemoved, this, &mqttpublisher::driverRemoved);
    connect(manager, &QTOZWManager::driverAllNodesQueried, this, &mqttpublisher::driverAllNodesQueried);
    connect(manager, &QTOZWManager::driverAwakeNodesQueried, this, &mqttpublisher::driverAwakeNodesQueried);
    connect(manager, &QTOZWManager::driverAllNodesQueriedSomeDead, this, &mqttpublisher::driverAllNodesQueriedSomeDead);
    connect(manager, &QTOZWManager::controllerCommand, this, &mqttpublisher::controllerCommand);
    connect(manager, &QTOZWManager::ozwNotification, this, &mqttpublisher::ozwNotification);
    connect(manager, &QTOZWManager::ozwUserAlert, this, &mqttpublisher::ozwUserAlert);
    connect(manager, &QTOZWManager::manufacturerSpecificDBReady, this, &mqttpublisher::manufacturerSpecificDBReady);
    connect(manager, &QTOZWManager::starting, this, &mqttpublisher::starting);
    connect(manager, &QTOZWManager::started, this, &mqttpublisher::started);
    connect(manager, &QTOZWManager::stopped, this, &mqttpublisher::stopped);
    
    this->m_currentStartTime = QDateTime::currentDateTime();
    if (settings->value("MQTTTLS").toBool() == true) {
        this->m_client->connectToHostEncrypted();   
    } else { 
        this->m_client->connectToHost();
    }
}

void mqttpublisher::updateLogStateChange()
{
    qCDebug(ozwmp) << "State Change" << m_client->state();
    if (settings->value("MQTTTLS").toBool() == true && this->m_client->state() == QMqttClient::ClientState::Connecting) {
        QSslSocket *socket = qobject_cast<QSslSocket *>(this->m_client->transport());
        socket->setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyNone);
    }
    if (this->m_client->state() == QMqttClient::ClientState::Connected) {
        this->m_cleanTopicSubscription = this->m_client->subscribe(QMqttTopicFilter(getTopic("#")));
        connect(this->m_cleanTopicSubscription, &QMqttSubscription::messageReceived, this, &mqttpublisher::cleanTopics);
        this->m_commands->setupSubscriptions(this->m_client, this->getCommandTopic());
    }

}

void mqttpublisher::brokerDisconnected()
{
    qCDebug(ozwmp) << "Disconnnected";
}

void mqttpublisher::handleMessage(const QByteArray &message, const QMqttTopicName &topic) {
//    qCDebug(ozwmp) << "Received: " << topic.name() << ":" << message;
}

bool mqttpublisher::sendStatusUpdate() {
    QT2JS::SetUInt64(this->m_ozwstatus, "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATUS_TOPIC)), QT2JS::getJSON(this->m_ozwstatus), 0, true);
    return true;
}

bool mqttpublisher::sendNodeUpdate(quint8 node) {
    QT2JS::SetUInt64(*this->m_nodes[node], "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), QT2JS::getJSON(*this->m_nodes[node]), 0, true);
    qDebug() << QT2JS::getJSON(*this->m_nodes[node]);
    return true;
}

bool mqttpublisher::sendValueUpdate(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    if (node == 0) {
        qCWarning(ozwmp) << "sendValueUpdate: Can't find Node for Value: " << vidKey;
        return false;
    }
    this->m_values[vidKey]["TimeStamp"] = QDateTime::currentSecsSinceEpoch(); 
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, vidKey)), QJsonDocument(this->m_values[vidKey]).toJson(), 0, true);
    return true;
}
void mqttpublisher::sendCommandUpdate(QString command, rapidjson::Document &js) {
    QT2JS::SetUInt64(js, "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getCommandResponseTopic(command.toLower())), QT2JS::getJSON(js), 0, false);
    return;
}

bool mqttpublisher::delNodeTopic(quint8 node) {
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), NULL, 0, false);
    return true;
}

bool mqttpublisher::delValueTopic(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    if (node == 0) {
        qCWarning(ozwmp) << "delValueTopic: Can't find Node for Value: " << vidKey;
        return false;
    }
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, vidKey)), NULL, 0, false);
    return true;
}


void mqttpublisher::ready() {
    qCDebug(ozwmp) << "Publishing Event ready:";
    QT2JS::SetString(this->m_ozwstatus, "Status", "Ready");
    this->sendStatusUpdate();
}
void mqttpublisher::valueAdded(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueAdded:" << vidKey;
    this->m_valueModel->populateJsonObject(&this->m_values[vidKey], vidKey, this->m_qtozwdeamon->getManager());
    this->m_values[vidKey]["Event"] = "valueAdded";
    this->sendValueUpdate(vidKey);
}
void mqttpublisher::valueRemoved(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueRemoved:" << vidKey;
    this->delValueTopic(vidKey);
}
void mqttpublisher::valueChanged(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueChanged:" << vidKey;
    this->m_values[vidKey]["Event"] = "valueChanged";
    this->m_values[vidKey]["Value"] = this->m_valueModel->encodeValue(vidKey);
    this->sendValueUpdate(vidKey);

}
void mqttpublisher::valueRefreshed(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueRefreshed:" << vidKey;
    this->m_values[vidKey]["Event"] = "valueRefreshed";
    this->m_values[vidKey]["Value"] = this->m_valueModel->encodeValue(vidKey);
    this->sendValueUpdate(vidKey);
}
void mqttpublisher::nodeNew(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event NodeNew:" << node;
    this->m_nodeModel->populateJsonObject(*this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeNew");
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeAdded(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event NodeAdded:" << node;
    if (this->m_nodes.find(node) == this->m_nodes.end()) {
        this->m_nodes.insert(node, new rapidjson::Document());
    }
    this->m_nodeModel->populateJsonObject(*this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeAdded");
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeRemoved(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event nodeRemoved:" << node;
    this->delNodeTopic(node);
    if (this->m_nodes.find(node) == this->m_nodes.end()) { 
        this->m_nodes.remove(node);
    } 
}
void mqttpublisher::nodeReset(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event nodeReset:" << node;
    this->delNodeTopic(node);
    if (this->m_nodes.find(node) == this->m_nodes.end()) { 
        this->m_nodes.remove(node);
    } 
}
void mqttpublisher::nodeNaming(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event nodeNaming:" << node;
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeNaming");
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeEvent(quint8 node, quint8 event) {
    Q_UNUSED(node);
    Q_UNUSED(event);
    /* we dont do anything here, as NodeEvent is just a BASIC message 
     * which should be handled via the normal ValueID/ValueChanged Events 
     */
}
void mqttpublisher::nodeProtocolInfo(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event nodeProtocolInfo:" << node;
    this->m_nodeModel->populateJsonObject(*this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeProtocolInfo");
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeEssentialNodeQueriesComplete(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event nodeEssentialNodeQueriesComplete:" << node;
    this->m_nodeModel->populateJsonObject(*this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeEssentialNodeQueriesComplete");
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeQueriesComplete(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event nodeQueriesComplete:" << node;
    this->m_nodeModel->populateJsonObject(*this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeQueriesComplete");
    this->sendNodeUpdate(node);
}
void mqttpublisher::driverReady(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event driverReady:" << homeID;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverReady");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
}
void mqttpublisher::driverFailed(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event driverFailed:" << homeID;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverFailed");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
}
void mqttpublisher::driverReset(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event driverReset:" << homeID;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverReset");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
}
void mqttpublisher::driverRemoved(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event driverRemoved:" << homeID;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverRemoved");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
}
void mqttpublisher::driverAllNodesQueriedSomeDead() {
    qCDebug(ozwmp) << "Publishing Event driverAllNodesQueriedSomeDead:" ;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverAllNodesQueriedSomeDead");
    this->sendStatusUpdate();
}
void mqttpublisher::driverAllNodesQueried() {
    qCDebug(ozwmp) << "Publishing Event driverAllNodesQueried:" ;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverAllNodesQueried");
    this->sendStatusUpdate();
}
void mqttpublisher::driverAwakeNodesQueried() {
    qCDebug(ozwmp) << "Publishing Event driverAwakeNodesQueried:" ;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverAwakeNodesQueried");
    this->sendStatusUpdate();
}
void mqttpublisher::controllerCommand(quint8 node, NotificationTypes::QTOZW_Notification_Controller_Cmd command, NotificationTypes::QTOZW_Notification_Controller_State state, NotificationTypes::QTOZW_Notification_Controller_Error error) {
    qCDebug(ozwmp) << "Publishing Event controllerCommand" << node << command << state << error;
    rapidjson::Document js;
    if (node > 0)
        QT2JS::SetUint(js, "Node", node);
    QMetaEnum metaEnum = QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_State>();
    QT2JS::SetString(js, "State", metaEnum.valueToKey(state));
    if (error != NotificationTypes::QTOZW_Notification_Controller_Error::Ctrl_Error_None) {
        metaEnum = QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Error>();
        QT2JS::SetString(js, "Error", metaEnum.valueToKey(error));
    }
        
    switch(command) {
        case NotificationTypes::Ctrl_Cmd_None: {
            qCWarning(ozwmp) << "Got a controllerCommand Event with no Controller Command" << command << state << error;
            break;
        }
        case NotificationTypes::Ctrl_Cmd_AddNode: {
            this->sendCommandUpdate("AddNode", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_AssignReturnRoute: {
            this->sendCommandUpdate("AssignReturnRoute", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_CreateButton: {
            QT2JS::SetString(js, "Command", QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Cmd>().valueToKey(command));
            this->sendCommandUpdate("ControllerCommand", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_CreateNewPrimary: {
            QT2JS::SetString(js, "Command", QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Cmd>().valueToKey(command));
            this->sendCommandUpdate("ControllerCommand", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_DeleteAllReturnRoute: {
            this->sendCommandUpdate("DeleteAllReturnRoute", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_DeleteButton: {
            QT2JS::SetString(js, "Command", QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Cmd>().valueToKey(command));
            this->sendCommandUpdate("ControllerCommand", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_HasNodeFailed: {
            this->sendCommandUpdate("HasNodeFailed", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_ReceiveConfiguration: {
            QT2JS::SetString(js, "Command", QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Cmd>().valueToKey(command));
            this->sendCommandUpdate("ControllerCommand", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_RemoveFailedNode: {
            this->sendCommandUpdate("RemoveFailedNode", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_RemoveNode: {
            this->sendCommandUpdate("RemoveNode", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_ReplaceFailedNode: {
            this->sendCommandUpdate("ReplaceFailedNode", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_ReplicationSend: {
            QT2JS::SetString(js, "Command", QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Cmd>().valueToKey(command));
            this->sendCommandUpdate("ControllerCommand", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_RequestNetworkUpdate: {
            this->sendCommandUpdate("RequestNetworkUpdate", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_RequestNodeNeighborUpdate: {
            this->sendCommandUpdate("RequestNodeNeighborUpdate", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_SendNodeInformation: {
            this->sendCommandUpdate("SendNodeInformation", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_TransferPrimaryRole: {
            QT2JS::SetString(js, "Command", QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Controller_Cmd>().valueToKey(command));
            this->sendCommandUpdate("ControllerCommand", js);
            break;
        }
        case NotificationTypes::Ctrl_Cmd_count: {
            qWarning() << "Recieved controllerCommand for a unknown Command:" << command;
            return;
        }
    };
}
void mqttpublisher::ozwNotification(quint8 node, NotificationTypes::QTOZW_Notification_Code event) {
    qCDebug(ozwmp) << "Publishing Event ozwNotification";
    rapidjson::Document js;
    QMetaEnum metaEnum = QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Code>();
    QT2JS::SetUint(js, "Node", node);
    QT2JS::SetString(js, "Event", metaEnum.valueToKey(event));
    this->sendCommandUpdate("Notification", js);
}
void mqttpublisher::ozwUserAlert(quint8 node, NotificationTypes::QTOZW_Notification_User event, quint8 retry) {
    qCDebug(ozwmp) << "Publishing Event ozwNotification";
    rapidjson::Document js;
    QMetaEnum metaEnum = QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_User>();
    QT2JS::SetUint(js, "Node", node);
    QT2JS::SetString(js, "Event", metaEnum.valueToKey(event));
    if (event == NotificationTypes::QTOZW_Notification_User::Notification_User_ApplicationStatus_Retry) {
        QT2JS::SetUint(js, "Retry", retry);
    }
    this->sendCommandUpdate("UserAlert", js);
}
void mqttpublisher::manufacturerSpecificDBReady() {
    qCDebug(ozwmp) << "Publishing Event manufacturerSpecificDBReady";
    QT2JS::SetBool(this->m_ozwstatus, "ManufacturerSpecificDBReady", true);
    this->sendStatusUpdate();
}

void mqttpublisher::starting() {
    qCDebug(ozwmp) << "Publishing Event starting";
    QT2JS::SetString(this->m_ozwstatus, "Status", "starting");
    this->sendStatusUpdate();
}
void mqttpublisher::started(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event started";
    QT2JS::SetString(this->m_ozwstatus, "Status", "started");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
    this->m_statsTimer.start(10000);
}
void mqttpublisher::stopped(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event stopped";
    QT2JS::SetString(this->m_ozwstatus, "Status", "stopped");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
    this->m_statsTimer.stop();
}
//void error(QTOZWErrorCodes errorcode);

QTOZWManager *mqttpublisher::getQTOZWManager() {
    return this->m_qtozwdeamon->getManager();
}
