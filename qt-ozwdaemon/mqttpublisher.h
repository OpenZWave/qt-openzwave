#ifndef MQTTPUBLISHER_H
#define MQTTPUBLISHER_H

#include <QObject>

#include <QtMqtt/QMqttClient>
#include <QTimer>
#include <rapidjson/document.h>

#include "qtozwdaemon.h"
#include "mqttcommands/mqttcommands.h"

class MqttCommands;

#define MQTT_OZW_TOP_TOPIC "OpenZWave/%1/"
#define MQTT_OZW_STATS_TOPIC "statistics/"
#define MQTT_OZW_STATS_NODE_TOPIC "node/%1/statistics/"
#define MQTT_OZW_STATUS_TOPIC "status/"
#define MQTT_OZW_NODE_TOPIC "node/%1/"
#define MQTT_OZW_VID_TOPIC "node/%1/value/%2/"
#define MQTT_OZW_COMMAND_TOPIC "command/%1/"
#define MQTT_OZW_RESPONSE_TOPIC "event/%1/"


class mqttNodeModel : public QTOZW_Nodes {
    Q_OBJECT
public:
    explicit mqttNodeModel(QObject *parent = nullptr);
    QVariant getNodeData(quint8, NodeColumns);
    bool populateJsonObject(rapidjson::Document &, quint8, QTOZWManager *);
    bool isValidNode(quint8);
};

class mqttValueIDModel : public QTOZW_ValueIds {
    Q_OBJECT
public:
    explicit mqttValueIDModel(QObject *parent = nullptr);
    QVariant getValueData(quint64, ValueIdColumns);
    bool populateJsonObject(rapidjson::Document &, quint64, QTOZWManager *);
    bool encodeValue(rapidjson::Document &, quint64);
    bool isValidValueID(quint64);
    bool setData(quint64, QVariant);
};

class mqttpublisher : public QObject
{
    Q_OBJECT
public:
    explicit mqttpublisher(QSettings *setting, QObject *parent = nullptr);
    void setOZWDaemon(qtozwdaemon *ozwdaemon);
    QTOZWManager *getQTOZWManager();
    void sendCommandUpdate(QString, rapidjson::Document &);
    bool isValidNode(quint8 node);
    bool isValidValueID(quint64 vidKey);
    QVariant getValueData(quint64, mqttValueIDModel::ValueIdColumns);
    bool setValue(quint64, QVariant);
signals:

public slots:
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
    void controllerCommand(quint8 node, NotificationTypes::QTOZW_Notification_Controller_Cmd command, NotificationTypes::QTOZW_Notification_Controller_State state, NotificationTypes::QTOZW_Notification_Controller_Error error);
    void ozwNotification(quint8 node, NotificationTypes::QTOZW_Notification_Code event);
    void ozwUserAlert(quint8 node, NotificationTypes::QTOZW_Notification_User event, quint8 retry);
    void manufacturerSpecificDBReady();

    void starting();
    void started(quint32 homeID);
    void stopped(quint32 homeID);
//    void error(QTOZWErrorCodes errorcode);

private slots:
    void updateLogStateChange();
    void brokerDisconnected();
    void doStats();
    void cleanTopics(QMqttMessage msg);
    void brokerError(QMqttClient::ClientError error);

private:

    QString getTopic(QString);
    QString getNodeTopic(QString, quint8);
    QString getValueTopic(QString, quint8, quint64);
    QString getCommandTopic();
    QString getCommandResponseTopic(QString);
    bool sendStatusUpdate();
    bool sendNodeUpdate(quint8);
    bool sendValueUpdate(quint64);
    bool delNodeTopic(quint8);
    bool delValueTopic(quint64);

    rapidjson::Document m_ozwstatus;
    QMap<quint8, rapidjson::Document *> m_nodes;
    mqttNodeModel *m_nodeModel;
    QMap<quint64, rapidjson::Document *> m_values;
    mqttValueIDModel *m_valueModel;

    QMqttClient *m_client;
    qtozwdaemon *m_qtozwdeamon;
    QSettings *settings;
    QTimer m_statsTimer;
    MqttCommands *m_commands;
    QMqttSubscription *m_cleanTopicSubscription;
    QDateTime m_currentStartTime;
};

#endif // MQTTPUBLISHER_H
