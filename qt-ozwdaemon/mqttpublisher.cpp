#include <QDateTime>

#include "mqttpublisher.h"

mqttNodeModel::mqttNodeModel(QObject *parent)
{

}

QVariant mqttNodeModel::getNodeData(quint8 node, NodeColumns col) {
    int row = this->getNodeRow(node);
    return this->data(this->index(row, col), Qt::DisplayRole);
}

bool mqttNodeModel::populateJsonObject(QJsonObject *jsonobject, quint8 node, QTOZWManager *mgr) {
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
                    jsonobject->insert(metaEnum.valueToKey(j), flag.at(j));
                }
                break;
            }
            default: {
                QMetaEnum metaEnum = QMetaEnum::fromType<NodeColumns>();
                if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QString) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toString());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Bool) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toBool());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Int) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::UInt) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
                } else {
                    qWarning() << "Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << node;
                }
                break;
            }
        }

    }
    /* MetaData */
    QJsonObject metadata = jsonobject->value("MetaData").toObject();
    QMetaEnum metaEnum = QMetaEnum::fromType<QTOZWManagerSource::QTOZWMetaDataField>();
    if (metadata.empty()) {
        for (int i = 0; i < QTOZWManagerSource::Identifier; i++) {
            metadata.insert(metaEnum.valueToKey(i), mgr->GetMetaData(node, static_cast<QTOZWManagerSource::QTOZWMetaDataField>(i)));
        }
        metadata.insert("ProductPicBase64", QString(mgr->GetMetaDataProductPic(node).toBase64()));
        jsonobject->insert("MetaData", metadata);
    }
    /* Neighbors */
    QVector<quint8> neighbors = mgr->GetNodeNeighbors(node);
    if (neighbors.size() > 0) {
        QJsonArray N;
        for (int i = 0; i < neighbors.count(); i++) {
            N.append(neighbors[i]);
        }
        jsonobject->insert("Neighbors", N);
    }
    return true;
}

mqttValueIDModel::mqttValueIDModel(QObject *parent) {

}
QVariant mqttValueIDModel::getValueData(quint64 vidKey, ValueIdColumns col) {
    int row = this->getValueRow(vidKey);
    return this->data(this->index(row, col), Qt::DisplayRole);
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
                qWarning() << "Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << vidKey;
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
    if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QString) {
        value = data.toString();
    } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Bool) {
        value = data.toBool();
    } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Int) {
        value = data.toInt();
    } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::UInt) {
        value = data.toInt();
    } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Float) {
        value = data.toDouble();
    } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::ULongLong) {
        value = static_cast<qint64>(data.toULongLong());
    } else if (static_cast<QMetaType::Type>(data.type())  == QMetaType::Short) {
        value = static_cast<qint16>(data.toInt());
    } else {
        qWarning() << "Can't Convert " << data.type() << " to store in JsonObject: " << vidKey << this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Label).toString() << this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Type);
    }
    return value;
}



mqttpublisher::mqttpublisher(QObject *parent) : QObject(parent)
{

    this->m_client = new QMqttClient(this);
    this->m_client->setHostname(settings.value("MQTTServer", "127.0.0.1").toString());
    this->m_client->setPort(static_cast<quint16>(settings.value("MQTTPort", 1883).toInt()));

    /* setup the Commands */
    this->m_commands =  new MqttCommands(this);
    this->m_commands->setupCommands();


    connect(this->m_client, &QMqttClient::stateChanged, this, &mqttpublisher::updateLogStateChange);
    connect(this->m_client, &QMqttClient::disconnected, this, &mqttpublisher::brokerDisconnected);

    connect(this->m_client, &QMqttClient::messageReceived, this, &mqttpublisher::handleMessage);
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
        qDebug() << content;
    });

    this->m_client->setWillTopic(getTopic(MQTT_OZW_STATUS_TOPIC));
    QJsonObject willMsg;
    willMsg["Status"] = "Offline";
    this->m_client->setWillMessage(QJsonDocument(willMsg).toJson());
    this->m_client->setWillRetain(true);
    connect(&this->m_statsTimer, &QTimer::timeout, this, &mqttpublisher::doStats);
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
    QJsonObject nodes;

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
        nsjson["lastSentTimeStamp"] = ns.lastSentTimeStamp; /**< TimeStamp of the Last time we sent a packet to the Node */
        nsjson["lastRecievedTimeStamp"] = ns.lastReceivedTimeStamp; /**< Timestamp of the last time we received a packet from the Node */
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
        nodes[QString(NodeID)] = nsjson;
    }
    stats["Nodes"] = nodes;
    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATS_TOPIC)), QJsonDocument(stats).toJson(), 0, false);

}

QString mqttpublisher::getTopic(QString topic) {
        return QString(MQTT_OZW_TOP_TOPIC).arg(settings.value("Instance", 1).toInt()).append(topic);
}

QString mqttpublisher::getNodeTopic(QString topic, quint8 node) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings.value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)));
    return t;
}
QString mqttpublisher::getValueTopic(QString topic, quint8 node, quint64 vid) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings.value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)).arg(static_cast<quint64>(vid)));
    return t;
}

QString mqttpublisher::getCommandTopic() {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings.value("Instance", 1).toInt());
    t.append(QString(MQTT_OZW_COMMAND_TOPIC));
    return t;
}

QString mqttpublisher::getCommandResponseTopic(QString cmd) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings.value("Instance", 1).toInt());
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

    this->m_client->connectToHost();
}

void mqttpublisher::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change: " )
                    + QString::number(m_client->state());
    qDebug() << content;
    if (this->m_client->state() == QMqttClient::ClientState::Connected) {
        this->m_client->subscribe(QMqttTopicFilter("/OpenZWave/commands"));
        this->m_commands->setupSubscriptions(this->m_client, this->getCommandTopic());
    }

}

void mqttpublisher::brokerDisconnected()
{
    qDebug() << "Disconnnected";
}

void mqttpublisher::handleMessage(const QByteArray &message, const QMqttTopicName &topic) {
    qDebug() << "Received: " << topic.name() << ":" << message;
}


bool mqttpublisher::sendStatusUpdate() {
    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATUS_TOPIC)), QJsonDocument(this->m_ozwstatus).toJson(), 0, true);
    return true;
}

bool mqttpublisher::sendNodeUpdate(quint8 node) {
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), QJsonDocument(this->m_nodes[node]).toJson(), 0, true);
    return true;
}

bool mqttpublisher::sendValueUpdate(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    if (node == 0) {
        qWarning() << "sendValueUpdate: Can't find Node for Value: " << vidKey;
        return false;
    }
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, vidKey)), QJsonDocument(this->m_values[vidKey]).toJson(), 0, true);
    return true;
}
void mqttpublisher::sendCommandUpdate(QString command, QJsonObject js) {
    this->m_client->publish(QMqttTopicName(getCommandResponseTopic(command.toLower())), QJsonDocument(js).toJson(), 0, false);
    return;
}

bool mqttpublisher::delNodeTopic(quint8 node) {
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), NULL, 0, false);
    return true;
}

bool mqttpublisher::delValueTopic(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    if (node == 0) {
        qWarning() << "delValueTopic: Can't find Node for Value: " << vidKey;
        return false;
    }
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, vidKey)), NULL, 0, false);
    return true;
}


void mqttpublisher::ready() {
    qDebug() << "Publishing Event ready:";
    this->m_ozwstatus["Status"] = "Ready";
    this->sendStatusUpdate();
}
void mqttpublisher::valueAdded(quint64 vidKey) {
    qDebug() << "Publishing Event valueAdded:" << vidKey;
    this->m_valueModel->populateJsonObject(&this->m_values[vidKey], vidKey, this->m_qtozwdeamon->getManager());
    this->m_values[vidKey]["Event"] = "valueAdded";
    this->sendValueUpdate(vidKey);
}
void mqttpublisher::valueRemoved(quint64 vidKey) {
    qDebug() << "Publishing Event valueRemoved:" << vidKey;
    this->delValueTopic(vidKey);
}
void mqttpublisher::valueChanged(quint64 vidKey) {
    qDebug() << "Publishing Event valueChanged:" << vidKey;
    this->m_values[vidKey]["Event"] = "valueChanged";
    this->m_values[vidKey]["Value"] = this->m_valueModel->encodeValue(vidKey);
    this->sendValueUpdate(vidKey);

}
void mqttpublisher::valueRefreshed(quint64 vidKey) {
    qDebug() << "Publishing Event valueRefreshed:" << vidKey;
    this->m_values[vidKey]["Event"] = "valueRefreshed";
    this->m_values[vidKey]["Value"] = this->m_valueModel->encodeValue(vidKey);
    this->sendValueUpdate(vidKey);
}
void mqttpublisher::nodeNew(quint8 node) {
    qDebug() << "Publishing Event NodeNew:" << node;
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeNew";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeAdded(quint8 node) {
    qDebug() << "Publishing Event NodeAdded:" << node;
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeAdded";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeRemoved(quint8 node) {
    qDebug() << "Publishing Event nodeRemoved:" << node;
    this->delNodeTopic(node);
}
void mqttpublisher::nodeReset(quint8 node) {
    qDebug() << "Publishing Event nodeReset:" << node;
    this->delNodeTopic(node);
}
void mqttpublisher::nodeNaming(quint8 node) {
    qDebug() << "Publishing Event nodeNaming:" << node;
    this->m_nodes[node]["Event"] = "nodeNaming";
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
    qDebug() << "Publishing Event nodeProtocolInfo:" << node;
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeProtocolInfo";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeEssentialNodeQueriesComplete(quint8 node) {
    qDebug() << "Publishing Event nodeEssentialNodeQueriesComplete:" << node;
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeEssentialNodeQueriesComplete";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeQueriesComplete(quint8 node) {
    qDebug() << "Publishing Event nodeQueriesComplete:" << node;
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeQueriesComplete";
    this->sendNodeUpdate(node);
}
void mqttpublisher::driverReady(quint32 homeID) {
    qDebug() << "Publishing Event driverReady:" << homeID;
    this->m_ozwstatus["Status"] = "driverReady";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverFailed(quint32 homeID) {
    qDebug() << "Publishing Event driverFailed:" << homeID;
    this->m_ozwstatus["Status"] = "driverFailed";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverReset(quint32 homeID) {
    qDebug() << "Publishing Event driverReset:" << homeID;
    this->m_ozwstatus["Status"] = "driverReset";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverRemoved(quint32 homeID) {
    qDebug() << "Publishing Event driverRemoved:" << homeID;
    this->m_ozwstatus["Status"] = "driverRemoved";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverAllNodesQueriedSomeDead() {
    qDebug() << "Publishing Event driverAllNodesQueriedSomeDead:" ;
    this->m_ozwstatus["Status"] = "driverAllNodesQueriedSomeDead";
    this->sendStatusUpdate();
}
void mqttpublisher::driverAllNodesQueried() {
    qDebug() << "Publishing Event driverAllNodesQueried:" ;
    this->m_ozwstatus["Status"] = "driverAllNodesQueried";
    this->sendStatusUpdate();
}
void mqttpublisher::driverAwakeNodesQueried() {
    qDebug() << "Publishing Event driverAwakeNodesQueried:" ;
    this->m_ozwstatus["Status"] = "driverAwakeNodesQueried";
    this->sendStatusUpdate();
}
void mqttpublisher::controllerCommand(quint8 node, NotificationTypes::QTOZW_Notification_Controller_Cmd command, NotificationTypes::QTOZW_Notification_Controller_State state, NotificationTypes::QTOZW_Notification_Controller_Error error) {
    qDebug() << "Publishing Event controllerCommand" << node << command << state << error;

}
void mqttpublisher::ozwNotification(quint8 node, NotificationTypes::QTOZW_Notification_Code event) {
    qDebug() << "Publishing Event ozwNotification";
    QJsonObject js;
    QMetaEnum metaEnum = QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_Code>();
    js["Node"] = node;
    js["Event"] = metaEnum.valueToKey(event);
    this->sendCommandUpdate("Notification", js);
}
void mqttpublisher::ozwUserAlert(quint8 node, NotificationTypes::QTOZW_Notification_User event, quint8 retry) {
    qDebug() << "Publishing Event ozwNotification";
    QJsonObject js;
    QMetaEnum metaEnum = QMetaEnum::fromType<NotificationTypes::QTOZW_Notification_User>();
    js["Node"] = node;
    js["Event"] = metaEnum.valueToKey(event);
    if (event == NotificationTypes::QTOZW_Notification_User::Notification_User_ApplicationStatus_Retry) {
        js["Retry"] = static_cast<int>(retry);
    }
    this->sendCommandUpdate("UserAlert", js);
}
void mqttpublisher::manufacturerSpecificDBReady() {
    qDebug() << "Publishing Event manufacturerSpecificDBReady";
    this->m_ozwstatus["ManufacturerSpecificDBReady"] = true;
    this->sendStatusUpdate();
}

void mqttpublisher::starting() {
    qDebug() << "Publishing Event starting";
    this->m_ozwstatus["Status"] = "starting";
    this->sendStatusUpdate();
}
void mqttpublisher::started(quint32 homeID) {
    qDebug() << "Publishing Event started";
    this->m_ozwstatus["Status"] = "started";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
    this->m_statsTimer.start(10000);
}
void mqttpublisher::stopped(quint32 homeID) {
    qDebug() << "Publishing Event stopped";
    this->m_ozwstatus["Status"] = "stopped";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
    this->m_statsTimer.stop();
}
//void error(QTOZWErrorCodes errorcode);

QTOZWManager *mqttpublisher::getQTOZWManager() {
    return this->m_qtozwdeamon->getManager();
}
