#include <QDateTime>

#include "mqttpublisher.h"
#include "qtrj.h"
#include "mqttcommands/mqttcommands.h"

#include <rapidjson/prettywriter.h> // for stringify JSON


Q_LOGGING_CATEGORY(ozwmp, "ozw.mqtt.publisher");

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
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
        qCDebug(ozwmp) << content;
    });

    this->m_client->setWillTopic(getTopic(MQTT_OZW_STATUS_TOPIC));
    rapidjson::Document willMsg(rapidjson::kObjectType);
    QT2JS::SetString(willMsg, "Status", "Offline");
    this->m_client->setWillMessage(QT2JS::getJSON(willMsg));
    this->m_client->setWillRetain(true);
    connect(&this->m_statsTimer, &QTimer::timeout, this, &mqttpublisher::doStats);
}

void mqttpublisher::cleanTopics(QMqttMessage msg) {
    if (msg.retain() == true) { 
        qCDebug(ozwmp) << "Topics: " << msg.topic().name();
        rapidjson::Document jsmsg;
        jsmsg.Parse(msg.payload());
        if (msg.topic().name() == getTopic("status")) {
            qCDebug(ozwmp) << msg.payload();
            /* when our status message is anything other than Offline, drop the Subscription */
            if (jsmsg.HasMember("Status") && (QString::fromStdString(jsmsg["Status"].GetString()) != "Offline")) {
                qCDebug(ozwmp) << "Unsubscribing from Topic Cleanup";
                this->m_cleanTopicSubscription->unsubscribe();
                exit(-1);
            }
            return;
        }
        if (jsmsg.HasMember("TimeStamp")) {
            QDateTime ts = QDateTime::fromSecsSinceEpoch(jsmsg["TimeStamp"].GetUint64());
            if (ts < this->m_currentStartTime) {
                qCDebug(ozwmp) << "Removing Stale Topic/Msg: " << msg.topic().name();
                this->m_client->publish(msg.topic(), "", 0, true);
            }
        } else {
            qCWarning(ozwmp) << "MQTT Message on Topic " << msg.topic().name() << "Does not have TimeStamp - Not Cleaning: " << msg.payload();
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
    if (!this->m_qtozwdeamon) {
        return; 
    }
    QTOZWManager *manager = this->m_qtozwdeamon->getManager();
    DriverStatistics ds = manager->GetDriverStatistics();
    rapidjson::Document dsjson;
    QT2JS::SetInt64(dsjson, "SOFCnt", ds.m_SOFCnt);
    QT2JS::SetInt64(dsjson, "ACKWaiting", ds.m_ACKWaiting);
    QT2JS::SetInt64(dsjson, "readAborts", ds.m_readAborts);
    QT2JS::SetInt64(dsjson, "badChecksum", ds.m_badChecksum);
    QT2JS::SetInt64(dsjson, "readCnt", ds.m_readCnt);
    QT2JS::SetInt64(dsjson, "writeCnt", ds.m_writeCnt);
    QT2JS::SetInt64(dsjson, "CANCnt", ds.m_CANCnt);
    QT2JS::SetInt64(dsjson, "NAKCnt", ds.m_NAKCnt);
    QT2JS::SetInt64(dsjson, "ACKCnt", ds.m_ACKCnt);
    QT2JS::SetInt64(dsjson, "OOFCnt", ds.m_OOFCnt);
    QT2JS::SetInt64(dsjson, "dropped", ds.m_dropped);
    QT2JS::SetInt64(dsjson, "retries", ds.m_retries);
    QT2JS::SetInt64(dsjson, "callbacks", ds.m_callbacks);
    QT2JS::SetInt64(dsjson, "badroutes", ds.m_badroutes);
    QT2JS::SetInt64(dsjson, "noack", ds.m_noack);
    QT2JS::SetInt64(dsjson, "netbusy", ds.m_netbusy);
    QT2JS::SetInt64(dsjson, "notidle", ds.m_notidle);
    QT2JS::SetInt64(dsjson, "txverified", ds.m_txverified);
    QT2JS::SetInt64(dsjson, "nondelivery", ds.m_nondelivery);
    QT2JS::SetInt64(dsjson, "routedbusy", ds.m_routedbusy);
    QT2JS::SetInt64(dsjson, "broadcastReadCnt", ds.m_broadcastReadCnt);
    QT2JS::SetInt64(dsjson, "broadcastWriteCnt", ds.m_broadcastWriteCnt);

    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATS_TOPIC)), QT2JS::getJSON(dsjson), 0, false);

    for (int i = 0; i < this->m_nodeModel->rowCount(QModelIndex()); i++) {
        rapidjson::Document nsjson;
        int NodeID = this->m_nodeModel->data(this->m_nodeModel->index(i, mqttNodeModel::NodeColumns::NodeID), Qt::DisplayRole).toInt();
        NodeStatistics ns = manager->GetNodeStatistics(NodeID);
        QT2JS::SetInt64(nsjson, "sendCount", ns.sentCount); /**< Number of Packets Sent to the Node */
        QT2JS::SetInt64(nsjson, "sentFailed", ns.sentFailed); /**< Number of Packets that Failed to be acknowledged by the Node or Controller */
        QT2JS::SetInt64(nsjson, "retries", ns.retries); /**< Number of times we have had to Retry sending packets to the Node */
        QT2JS::SetInt64(nsjson, "receivedPackets", ns.receivedPackets); /**< Number of received Packets from the Node */
        QT2JS::SetInt64(nsjson, "receivedDupPackets", ns.receivedDupPackets); /**< Number of Duplicate Packets received from the Node */
        QT2JS::SetInt64(nsjson, "receivedUnsolicited", ns.receivedUnsolicited); /**< Number of Unsolicited Packets received from the Node */
        QT2JS::SetInt64(nsjson, "lastSentTimeStamp", ns.lastSentTimeStamp.toSecsSinceEpoch()); /**< TimeStamp of the Last time we sent a packet to the Node */
        QT2JS::SetInt64(nsjson, "lastReceivedTimeStamp", ns.lastReceivedTimeStamp.toSecsSinceEpoch()); /**< Timestamp of the last time we received a packet from the Node */
        QT2JS::SetInt64(nsjson, "lastRequestRTT", ns.lastRequestRTT); /**<  Last Round-Trip Time when we made a request to the Node */
        QT2JS::SetInt64(nsjson, "averageRequestRTT", ns.averageRequestRTT); /**< Average Round-Trip Time when we make requests to a Node */
        QT2JS::SetInt64(nsjson, "lastResponseRTT", ns.lastResponseRTT); /**< Last Round-Trip Time when we got a Response from a Node */
        QT2JS::SetInt64(nsjson, "averageResponseRTT", ns.averageResponseRTT); /**< Average Round-Trip Time when got a Response from a Node */
        QT2JS::SetInt(nsjson, "quality", ns.quality); /**< The Quality of the Signal from the Node, as Reported by the Controller */
        QT2JS::SetBool(nsjson, "extendedTXSupported", ns.extendedTXSupported); /**< If these statistics support Extended TX Reporting (Controller Dependent) */
        QT2JS::SetInt(nsjson, "txTime", ns.txTime); /**< The Time it took to Transmit the last packet to the Node */
        QT2JS::SetInt(nsjson, "hops", ns.hops); /**< The Number of hops the packet traversed to reach the node */
        QT2JS::SetString(nsjson, "rssi_1", ns.rssi_1); /**< The RSSI Strength of the first hop */
        QT2JS::SetString(nsjson, "rssi_2", ns.rssi_2); /**< The RSSI Strength of the second hop */
        QT2JS::SetString(nsjson, "rssi_3", ns.rssi_3); /**< The RSSI Strength of the third hop */
        QT2JS::SetString(nsjson, "rssi_4", ns.rssi_4); /**< The RSSI Strength of the fourth hop */
        QT2JS::SetString(nsjson, "rssi_5", ns.rssi_5); /**< The RSSI Strength of the final hop */
        QT2JS::SetInt(nsjson, "route_1", ns.route_1); /**< The NodeId of the First Hop */
        QT2JS::SetInt(nsjson, "route_2", ns.route_2); /**< The NodeId of the Second Hop */
        QT2JS::SetInt(nsjson, "route_3", ns.route_3); /**< The NodeId of the third Hop */
        QT2JS::SetInt(nsjson, "route_4", ns.route_4); /**< The NodeId of the Fourth Hop */
        QT2JS::SetInt(nsjson, "ackChannel", ns.ackChannel); /**< The Channel that recieved the ACK From the Node */
        QT2JS::SetInt(nsjson, "lastTXChannel", ns.lastTXChannel); /**< The last Channel we used to communicate with the Node */
        QT2JS::SetString(nsjson, "routeScheme", ns.routeScheme); /**< How the Route was calculated when we last communicated with the Node */
        QT2JS::SetString(nsjson, "routeUsed", ns.routeUsed); /**< The Route last used to communicate with the Node */
        QT2JS::SetString(nsjson, "routeSpeed", ns.routeSpeed); /**< The Speed that was used when we last communicated with the node */
        QT2JS::SetInt(nsjson, "routeTries", ns.routeTries); /**< The Number of attempts the Controller made to route the packet to the Node */
        QT2JS::SetInt(nsjson, "lastFailedLinkFrom", ns.lastFailedLinkFrom); /**< The Last Failed Link From */
        QT2JS::SetInt(nsjson, "lastFailedLinkTo", ns.lastFailedLinkTo); /**< The Last Failed Link To */
        this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_STATS_NODE_TOPIC, NodeID)), QT2JS::getJSON(nsjson), 0, false);
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

QString mqttpublisher::getInstanceTopic(QString topic, quint8 node, quint8 instance) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)).arg(static_cast<quint8>(instance)));
    return t;
}

QString mqttpublisher::getCommandClassTopic(QString topic, quint8 node, quint8 instance, quint8 cc) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)).arg(static_cast<quint8>(instance)).arg(static_cast<quint8>(cc)));
    return t;
}

QString mqttpublisher::getValueTopic(QString topic, quint8 node, quint8 instance, quint8 cc, quint64 vid) {
    QString t(MQTT_OZW_TOP_TOPIC);
    t = t.arg(settings->value("Instance", 1).toInt());
    t.append(topic.arg(static_cast<quint8>(node)).arg(static_cast<quint8>(instance)).arg(static_cast<quint8>(cc)).arg(static_cast<quint64>(vid)));
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

bool mqttpublisher::sendStatusUpdate() {
    QT2JS::SetUInt64(this->m_ozwstatus, "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATUS_TOPIC)), QT2JS::getJSON(this->m_ozwstatus), 0, true);
    return true;
}

bool mqttpublisher::sendNodeUpdate(quint8 node) {
    QT2JS::SetUInt64(*this->m_nodes[node], "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), QT2JS::getJSON(*this->m_nodes[node]), 0, true);
    return true;
}

bool mqttpublisher::sendValueUpdate(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    quint8 instance = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Instance).value<quint8>();
    quint8 cc = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::CommandClass).value<quint8>();
    if (node == 0) {
        qCWarning(ozwmp) << "sendValueUpdate: Can't find Node for Value: " << vidKey;
        return false;
    }
    if (instance == 0) {
        qCWarning(ozwmp) << "sendValueUpdate: Can't find instance for Value: " << vidKey;
        return false;
    }
    if (cc == 0) {
        qCWarning(ozwmp) << "sendValueUpdate: Can't find CC for Value: " << vidKey;
        return false;
    }
    QT2JS::SetUInt64(*this->m_values[vidKey], "TimeStamp", QDateTime::currentSecsSinceEpoch()); 
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, instance, cc, vidKey)), QT2JS::getJSON(*this->m_values[vidKey]), 0, true);
    return true;
}

bool mqttpublisher::sendInstanceUpdate(quint8 node, quint8 instance) {
    rapidjson::Document *jsinstance = nullptr;
    if (!(jsinstance = this->getInstanceJSON(node, instance))) {
        return false;
    }
    QT2JS::SetUInt64(*jsinstance, "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getInstanceTopic(MQTT_OZW_INSTANCE_TOPIC, node, instance)), QT2JS::getJSON(*jsinstance), 0, true);
    return true;
}

bool mqttpublisher::sendCommandClassUpdate(quint8 node, quint8 instance, quint8 cc) {
    rapidjson::Document *jsCommandClass = nullptr;
    if (!(jsCommandClass = this->getCommandClassJSON(node, instance, cc))) {
        return false;
    }
    QT2JS::SetUInt64(*jsCommandClass, "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getCommandClassTopic(MQTT_OZW_COMMANDCLASS_TOPIC, node, instance, cc)), QT2JS::getJSON(*jsCommandClass), 0, true);
    return true;
}

void mqttpublisher::sendCommandUpdate(QString command, rapidjson::Document &js) {
    QT2JS::SetUInt64(js, "TimeStamp", QDateTime::currentSecsSinceEpoch());
    this->m_client->publish(QMqttTopicName(getCommandResponseTopic(command.toLower())), QT2JS::getJSON(js), 0, false);
    return;
}

bool mqttpublisher::delNodeTopic(quint8 node) {
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), NULL, 0, true);
    return true;
}

bool mqttpublisher::delValueTopic(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    quint8 instance = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Instance).value<quint8>();
    quint8 cc = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::CommandClass).value<quint8>();
    if (node == 0) {
        qCWarning(ozwmp) << "delValueTopic: Can't find Node for Value: " << vidKey;
        return false;
    }
    if (instance == 0) {
        qCWarning(ozwmp) << "sendValueUpdate: Can't find instance for Value: " << vidKey;
        return false;
    }
    if (cc == 0) {
        qCWarning(ozwmp) << "sendValueUpdate: Can't find CC for Value: " << vidKey;
        return false;
    }
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, instance, cc, vidKey)), NULL, 0, true);
    return true;
}

bool mqttpublisher::delInstanceTopic(quint8 node, quint8 instance) {
    this->m_client->publish(QMqttTopicName(getInstanceTopic(MQTT_OZW_INSTANCE_TOPIC, node, instance)), NULL, 0, true);
    return true;
}

bool mqttpublisher::delCommandClassTopic(quint8 node, quint8 instance, quint8 cc) {
    this->m_client->publish(QMqttTopicName(getCommandClassTopic(MQTT_OZW_COMMANDCLASS_TOPIC, node, instance, cc)), NULL, 0, true);
    return true;
}

void mqttpublisher::ready() {
    qCDebug(ozwmp) << "Publishing Event ready:";
    QT2JS::SetString(this->m_ozwstatus, "Status", "Ready");
    this->sendStatusUpdate();
}
void mqttpublisher::valueAdded(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueAdded:" << vidKey;
    /* create instance and CC Topics if necessary */
    rapidjson::Document *jsinstance = nullptr;
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    quint8 instance = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Instance).value<quint8>();
    quint8 cc = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::CommandClass).value<quint8>();

    if (!(jsinstance = this->getInstanceJSON(node, instance))) {
        jsinstance = new rapidjson::Document(rapidjson::kObjectType);
        QT2JS::SetInt(*jsinstance, "Instance", instance);
        this->m_instances[node][instance] = jsinstance;            
    }
    this->sendInstanceUpdate(node, instance);

    rapidjson::Document *jsCommandClass = nullptr;
    if (!(jsCommandClass = this->getCommandClassJSON(node, instance, cc))) {
        jsCommandClass = new rapidjson::Document(rapidjson::kObjectType);
        QT2JS::SetInt(*jsCommandClass, "Instance", instance);
        QT2JS::SetInt(*jsCommandClass, "CommandClassId", cc);
        QT2JS::SetString(*jsCommandClass, "CommandClass", this->getQTOZWManager()->getCommandClassString(cc));
        this->m_CommandClasses[node][instance][cc] = jsCommandClass;
    }
    this->sendCommandClassUpdate(node, instance, cc);

    if (this->m_values.find(vidKey) == this->m_values.end()) {
        this->m_values.insert(vidKey, new rapidjson::Document());
    }
    if (this->m_valueModel->populateJsonObject(*this->m_values[vidKey], vidKey, this->m_qtozwdeamon->getManager())) {
        /* something has changed */
        QT2JS::SetString(*this->m_values[vidKey], "Event", "valueAdded");
        this->sendValueUpdate(vidKey);
    }
}
void mqttpublisher::valueRemoved(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueRemoved:" << vidKey;
    this->delValueTopic(vidKey);
    quint8 vinstance = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Instance).toInt();
    quint8 vcc = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::CommandClass).toInt();
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node).toInt();
    bool removeInstance = true;
    bool removeCC = true;
    QMap<quint64, rapidjson::Document *>::iterator it;
    for (it =this->m_values.begin(); it != this->m_values.end(); it++) {
        if (this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::ValueIDKey).value<quint64>() == vidKey) {
            continue;
        }
        if (this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::Node).toInt() != node) {
            continue;
        }
        if (this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::Instance).toInt() != vinstance) {
            continue;
        };
        quint8 cc = this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::CommandClass).toInt();
        if (vcc == cc) {
            removeCC = false;
        }
    }
    if (removeCC) {
        qCDebug(ozwmp) << "Removing CommandClass Topic for " << this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node).toInt() << vinstance << vcc;
        this->delCommandClassTopic(this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node).toInt(), vinstance, vcc);
    }
    for (it =this->m_values.begin(); it != this->m_values.end(); it++) {
        if (this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::ValueIDKey).value<quint64>() == vidKey) {
            continue;
        }
        if (this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::Node).toInt() != node) {
            continue;
        }
        quint8 instance = this->m_valueModel->getValueData(it.key(), QTOZW_ValueIds::ValueIdColumns::Instance).toInt();
        if (vinstance == instance) {
            removeInstance = false;
        }
    }

    if (removeInstance) {
        qCDebug(ozwmp) << "Removing Instance Topic for " << this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node).toInt() << vinstance;
        this->delInstanceTopic(this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Node).toInt(), vinstance);
    }


    if (this->m_values.find(vidKey) != this->m_values.end()) {
        this->m_values.remove(vidKey);
    } else {
        qCWarning(ozwmp) << "Can't Find Value Map for " << vidKey;
    }
}
void mqttpublisher::valueChanged(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueChanged:" << vidKey;
    if (this->m_valueModel->encodeValue(*this->m_values[vidKey], vidKey)) {
        /* something has changed */
        QT2JS::SetString(*this->m_values[vidKey], "Event", "valueChanged");
        this->sendValueUpdate(vidKey);
    }
}
void mqttpublisher::valueRefreshed(quint64 vidKey) {
    qCDebug(ozwmp) << "Publishing Event valueRefreshed:" << vidKey;
    if (this->m_valueModel->encodeValue(*this->m_values[vidKey], vidKey)) {
        /* something has changed */
        QT2JS::SetString(*this->m_values[vidKey], "Event", "valueRefreshed");
        this->sendValueUpdate(vidKey);
    }
}
void mqttpublisher::nodeNew(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event NodeNew:" << node;
    if (this->m_nodes.find(node) == this->m_nodes.end()) {
        this->m_nodes.insert(node, new rapidjson::Document(rapidjson::kObjectType));
    }
    this->m_nodeModel->populateJsonObject(*this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    QT2JS::SetString(*this->m_nodes[node], "Event", "nodeNew");
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeAdded(quint8 node) {
    qCDebug(ozwmp) << "Publishing Event NodeAdded:" << node;
    if (this->m_nodes.find(node) == this->m_nodes.end()) {
        this->m_nodes.insert(node, new rapidjson::Document(rapidjson::kObjectType));
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
    /* XXX TODO: Scan Nodes, Instances, CC and Value Lists and delete them */
}
void mqttpublisher::driverReset(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event driverReset:" << homeID;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverReset");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
    /* XXX TODO: Scan Nodes, Instances, CC and Value Lists and delete them */
}
void mqttpublisher::driverRemoved(quint32 homeID) {
    qCDebug(ozwmp) << "Publishing Event driverRemoved:" << homeID;
    QT2JS::SetString(this->m_ozwstatus, "Status", "driverRemoved");
    QT2JS::SetUint(this->m_ozwstatus, "homeID", homeID);
    this->sendStatusUpdate();
    /* XXX TODO: Scan Nodes, Instances, CC and Value Lists and delete them */
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
    qCDebug(ozwmp) << "Publishing Event ozwUserAlert";
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

rapidjson::Document *mqttpublisher::getInstanceJSON(quint8 node, quint8 instance) {
    if (this->m_instances.find(node) != this->m_instances.end()) {
        if (this->m_instances[node].find(instance) != this->m_instances[node].end()) {
            return this->m_instances[node][instance];
        }
    }
    return nullptr;
}
rapidjson::Document *mqttpublisher::getCommandClassJSON(quint8 node, quint8 instance, quint8 cc) {
    if (this->m_CommandClasses.find(node) != this->m_CommandClasses.end()) {
        if (this->m_CommandClasses[node].find(instance) != this->m_CommandClasses[node].end()) {
            if (this->m_CommandClasses[node][instance].find(cc) != this->m_CommandClasses[node][instance].end()) {
                return this->m_CommandClasses[node][instance][cc]; 
            }
        }
    }
    return nullptr;
}
