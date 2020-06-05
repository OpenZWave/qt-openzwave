#ifndef MQTTPUBLISHER_H
#define MQTTPUBLISHER_H

#include <QObject>

#include <QtMqtt/QMqttClient>
#include <QTimer>
#include <rapidjson/document.h>

#include "qtozwdaemon.h"
#include "mqttNodes.h"
#include "mqttValues.h"
#include "mqttAssociations.h"
#include "mqttcommands/mqttcommands.h"

class MqttCommands;

#define MQTT_OZW_TOP_TOPIC "OpenZWave/%1/"
#define MQTT_OZW_STATS_TOPIC "statistics/"
#define MQTT_OZW_STATS_NODE_TOPIC "node/%1/statistics/"
#define MQTT_OZW_STATUS_TOPIC "status/"
#define MQTT_OZW_NODE_TOPIC "node/%1/"
#define MQTT_OZW_INSTANCE_TOPIC "node/%1/instance/%2/"
#define MQTT_OZW_VID_TOPIC "node/%1/instance/%2/commandclass/%3/value/%4/"
#define MQTT_OZW_COMMANDCLASS_TOPIC "node/%1/instance/%2/commandclass/%3/"
#define MQTT_OZW_ASSOCIATION_TOPIC "node/%1/association/%2/"
#define MQTT_OZW_COMMAND_TOPIC "command/%1/"
#define MQTT_OZW_RESPONSE_TOPIC "event/%1/"


class mqttpublisher : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(bool ready READ isReady WRITE setReady NOTIFY readyChanged);

    explicit mqttpublisher(QSettings *setting, QObject *parent = nullptr);
    ~mqttpublisher();
    void setOZWDaemon(qtozwdaemon *ozwdaemon);
    QTOZWManager *getQTOZWManager();
    bool sendCommandUpdate(QString, rapidjson::Document &);
    bool sendAssociationUpdate(quint8 node, quint8 group, rapidjson::Document &js);
    bool isValidNode(quint8 node);
    bool isValidValueID(quint64 vidKey);
    QVariant getValueData(quint64, mqttValueIDModel::ValueIdColumns);
    bool setValue(quint64, QVariant);
    bool isReady();
    void setReady(bool ready);
signals:
    void readyChanged(bool ready);

public slots:
    void ready(bool);
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
    void nodeGroupChanged(quint8 node, quint8 group);
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
    QString getInstanceTopic(QString, quint8, quint8);
    QString getCommandClassTopic(QString, quint8, quint8, quint8);
    QString getValueTopic(QString, quint8, quint8, quint8, quint64);
    QString getAssociationTopic(quint8, quint8);
    QString getCommandTopic();
    QString getCommandResponseTopic(QString);
    bool sendStatusUpdate();
    bool clearStatusUpdate();
    bool sendNodeUpdate(quint8);
    bool sendValueUpdate(quint64);
    bool sendInstanceUpdate(quint8, quint8);
    bool sendCommandClassUpdate(quint8, quint8, quint8);

    bool delNodeTopic(quint8);
    bool delValueTopic(quint64);
    bool delInstanceTopic(quint8, quint8);
    bool delCommandClassTopic(quint8, quint8, quint8);
    bool delAssociationTopic(quint8, quint8);
    rapidjson::Document *getInstanceJSON(quint8, quint8);
    rapidjson::Document *getCommandClassJSON(quint8, quint8, quint8);

    rapidjson::Document m_ozwstatus;
    QMap<quint8, rapidjson::Document *> m_nodes;
    QMap<quint8, QMap<quint8, rapidjson::Document *> > m_instances;
    QMap<quint8, QMap<quint8, QMap <quint8, rapidjson::Document *> > > m_CommandClasses;
    QMap<quint8, QVector<quint8> > m_assoications;
    mqttNodeModel *m_nodeModel;
    QMap<quint64, rapidjson::Document *> m_values;
    mqttValueIDModel *m_valueModel;
    mqttAssociationModel *m_assocModel;

    QMqttClient *m_client;
    qtozwdaemon *m_qtozwdaemon;
    QSettings *settings;
    QTimer m_statsTimer;
    MqttCommands *m_commands;
    QMqttSubscription *m_cleanTopicSubscription;
    QDateTime m_currentStartTime;
    bool m_ready;
};

#endif // MQTTPUBLISHER_H
