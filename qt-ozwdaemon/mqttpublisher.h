#ifndef MQTTPUBLISHER_H
#define MQTTPUBLISHER_H

#include <QObject>

#include <QtMqtt/QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>

#include "qtozwdaemon.h"


#define MQTT_OZW_TOP_TOPIC "/OpenZWave/%1/"
#define MQTT_OZW_STATUS_TOPIC "status/"
#define MQTT_OZW_NODE_TOPIC "node/%1/"
#define MQTT_OZW_VID_TOPIC "node/%1/%2/"

class mqttNodeModel : public QTOZW_Nodes {
    Q_OBJECT
public:
    explicit mqttNodeModel(QObject *parent = nullptr);

    QVariant getNodeData(quint8, NodeColumns);
    bool populateJsonObject(QJsonObject *, quint8, QTOZWManager *);
};

class mqttValueIDModel : public QTOZW_ValueIds {
    Q_OBJECT
public:
    explicit mqttValueIDModel(QObject *parent = nullptr);
    QVariant getValueData(quint64, ValueIdColumns);
    bool populateJsonObject(QJsonObject *, quint64, QTOZWManager *);
    QJsonValue encodeValue(quint64);
};

class mqttpublisher : public QObject
{
    Q_OBJECT
public:
    explicit mqttpublisher(QObject *parent = nullptr);
    void setOZWDaemon(qtozwdaemon *ozwdaemon);
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
    void controllerCommand(quint8 command);
//    void ozwNotification(OpenZWave::Notification::NotificationCode event);
// void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void manufacturerSpecificDBReady();

    void starting();
    void started(quint32 homeID);
    void stopped(quint32 homeID);
//    void error(QTOZWErrorCodes errorcode);




private slots:
    void updateLogStateChange();
    void brokerDisconnected();
    void handleMessage(const QByteArray &message, const QMqttTopicName &topic = QMqttTopicName());

private:

    QString getTopic(QString);
    QString getNodeTopic(QString, quint8);
    QString getValueTopic(QString, quint8, quint64);
    bool sendStatusUpdate();
    bool sendNodeUpdate(quint8);
    bool sendValueUpdate(quint64);

    QJsonObject m_ozwstatus;
    QMap<quint8, QJsonObject> m_nodes;
    mqttNodeModel *m_nodeModel;
    QMap<quint64, QJsonObject> m_values;
    mqttValueIDModel *m_valueModel;

    QMqttClient *m_client;
    qtozwdaemon *m_qtozwdeamon;
    QSettings settings;
};

#endif // MQTTPUBLISHER_H