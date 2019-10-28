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
                if (data.type() == QMetaType::QString) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toString());
                } else if (data.type() == QMetaType::Bool) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toBool());
                } else if (data.type() == QMetaType::Int) {
                    jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
                } else if (data.type() == QMetaType::UInt) {
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
    qDebug() << jsonobject;
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
        default: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdColumns>();
            if (data.type() == QMetaType::QString) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toString());
            } else if (data.type() == QMetaType::Bool) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toBool());
            } else if (data.type() == QMetaType::Int) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
            } else if (data.type() == QMetaType::UInt) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toInt());
            } else if (data.type() == QMetaType::Float) {
                jsonobject->insert(metaEnum.valueToKey(i), data.toDouble());
            } else if (data.type() == QMetaType::ULongLong) {
                jsonobject->insert(metaEnum.valueToKey(i), static_cast<qint64>(data.toULongLong()));
            } else {
                qWarning() << "Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << vidKey;
            }
            break;
        }

        }
    }


    qDebug() << jsonobject;
    return true;
}

QJsonValue mqttValueIDModel::encodeValue(quint64 vidKey) {
    QJsonValue value;
    QVariant data = this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Value);
    if (data.type() == QMetaType::QString) {
        value = data.toString();
    } else if (data.type() == QMetaType::Bool) {
        value = data.toBool();
    } else if (data.type() == QMetaType::Int) {
        value = data.toInt();
    } else if (data.type() == QMetaType::UInt) {
        value = data.toInt();
    } else if (data.type() == QMetaType::Float) {
        value = data.toDouble();
    } else if (data.type() == QMetaType::ULongLong) {
        value = static_cast<qint64>(data.toULongLong());
    } else {
        qWarning() << "Can't Convert " << data.type() << " to store in JsonObject: " << vidKey;
    }
    return value;
}



mqttpublisher::mqttpublisher(QObject *parent) : QObject(parent)
{

    this->m_client = new QMqttClient(this);
    this->m_client->setHostname(settings.value("MQTTServer", "127.0.0.1").toString());
    this->m_client->setPort(static_cast<quint16>(settings.value("MQTTPort", 1883).toInt()));

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
    qDebug() << QJsonDocument(this->m_ozwstatus).toJson();
    this->m_client->publish(QMqttTopicName(getTopic(MQTT_OZW_STATUS_TOPIC)), QJsonDocument(this->m_ozwstatus).toJson(), 0, true);
    return true;
}

bool mqttpublisher::sendNodeUpdate(quint8 node) {
    qDebug() << this->m_nodeModel->getNodeData(node, QTOZW_Nodes::NodeProductName);
    this->m_client->publish(QMqttTopicName(getNodeTopic(MQTT_OZW_NODE_TOPIC, node)), QJsonDocument(this->m_nodes[node]).toJson(), 0, true);
    return true;
}

bool mqttpublisher::sendValueUpdate(quint64 vidKey) {
    quint8 node = this->m_valueModel->getValueData(vidKey, QTOZW_ValueIds::Node).value<quint8>();
    if (node == 0)
        return false;
    this->m_client->publish(QMqttTopicName(getValueTopic(MQTT_OZW_VID_TOPIC, node, vidKey)), QJsonDocument(this->m_values[vidKey]).toJson(), 0, true);
    return true;
}


void mqttpublisher::ready() {
    this->m_ozwstatus["Status"] = "Ready";
    this->sendStatusUpdate();
}
void mqttpublisher::valueAdded(quint64 vidKey) {
    this->m_valueModel->populateJsonObject(&this->m_values[vidKey], vidKey, this->m_qtozwdeamon->getManager());
    this->m_values[vidKey]["Event"] = "valueAdded";
    this->sendValueUpdate(vidKey);
}
void mqttpublisher::valueRemoved(quint64 vidKey) {
    this->m_values[vidKey]["Event"] = "valueRemoved";
    this->sendValueUpdate(vidKey);

}
void mqttpublisher::valueChanged(quint64 vidKey) {
    this->m_values[vidKey]["Event"] = "valueChanged";
    this->m_values[vidKey]["Value"] = this->m_valueModel->encodeValue(vidKey);
    this->sendValueUpdate(vidKey);

}
void mqttpublisher::valueRefreshed(quint64 vidKey) {
    this->m_values[vidKey]["Event"] = "valueRefreshed";
    this->m_values[vidKey]["Value"] = this->m_valueModel->encodeValue(vidKey);
    this->sendValueUpdate(vidKey);
}
void mqttpublisher::nodeNew(quint8 node) {
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeNew";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeAdded(quint8 node) {
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeAdded";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeRemoved(quint8 node) {
    this->m_nodes[node]["Event"] = "nodeRemoved";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeReset(quint8 node) {
    this->m_nodes[node]["Event"] = "nodeReset";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeNaming(quint8 node) {
    this->m_nodes[node]["Event"] = "nodeNaming";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeEvent(quint8 node, quint8 event) {
    this->m_nodes[node]["Event"] = "nodeEvent";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeProtocolInfo(quint8 node) {
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeProtocolInfo";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeEssentialNodeQueriesComplete(quint8 node) {
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeEssentialNodeQueriesComplete";
    this->sendNodeUpdate(node);
}
void mqttpublisher::nodeQueriesComplete(quint8 node) {
    this->m_nodeModel->populateJsonObject(&this->m_nodes[node], node, this->m_qtozwdeamon->getManager());
    this->m_nodes[node]["Event"] = "nodeQueriesComplete";
    this->sendNodeUpdate(node);
}
void mqttpublisher::driverReady(quint32 homeID) {
    this->m_ozwstatus["Status"] = "driverReady";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverFailed(quint32 homeID) {
    this->m_ozwstatus["Status"] = "driverFailed";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverReset(quint32 homeID) {
    this->m_ozwstatus["Status"] = "driverReset";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverRemoved(quint32 homeID) {
    this->m_ozwstatus["Status"] = "driverRemoved";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::driverAllNodesQueriedSomeDead() {
    this->m_ozwstatus["Status"] = "driverAllNodesQueriedSomeDead";
    this->sendStatusUpdate();
}
void mqttpublisher::driverAllNodesQueried() {
    this->m_ozwstatus["Status"] = "driverAllNodesQueried";
    this->sendStatusUpdate();
}
void mqttpublisher::driverAwakeNodesQueried() {
    this->m_ozwstatus["Status"] = "driverAwakeNodesQueried";
    this->sendStatusUpdate();
}
void mqttpublisher::controllerCommand(quint8 command) {

}
//void ozwNotification(OpenZWave::Notification::NotificationCode event);
//void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
void mqttpublisher::manufacturerSpecificDBReady() {

}

void mqttpublisher::starting() {
    this->m_ozwstatus["Status"] = "starting";
    qDebug() << QJsonDocument(this->m_ozwstatus).toJson();
    this->sendStatusUpdate();
}
void mqttpublisher::started(quint32 homeID) {
    this->m_ozwstatus["Status"] = "started";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
void mqttpublisher::stopped(quint32 homeID) {
    this->m_ozwstatus["Status"] = "stopped";
    this->m_ozwstatus["homeID"] = QJsonValue(static_cast<int>(homeID));
    this->sendStatusUpdate();
}
//void error(QTOZWErrorCodes errorcode);

