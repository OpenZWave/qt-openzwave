#include "mqttcommands/mqttcommands.h"
#include "mqttcommands/ping.h"


MqttCommand::MqttCommand(QObject *parent) :
    QObject(parent)
{
}

void MqttCommand::Setup(QMqttSubscription *subscription) {
    this->m_subscription = subscription;
    qDebug() << "Subscription Setup for " << this->m_subscription->topic();
    connect(this->m_subscription, &QMqttSubscription::messageReceived, this, &MqttCommand::messageReceived);
    connect(this, &MqttCommand::sendCommandUpdate, getMqttPublisher(), &mqttpublisher::sendCommandUpdate);
}

QTOZWManager *MqttCommand::getOZWManager() {
    mqttpublisher *mqttp = getMqttPublisher();
    if (mqttp) {
        return mqttp->getQTOZWManager();
    }
    return nullptr;
}
mqttpublisher *MqttCommand::getMqttPublisher() {
    if (this->parent()) {
        mqttpublisher *mqttp = qobject_cast<mqttpublisher*>(this->parent());
        return mqttp;
    }
    return nullptr;
}

void MqttCommand::messageReceived(QMqttMessage msg) {
    qDebug() << "Got "<< msg.topic().name()<< " Message: " << msg.payload();
    QJsonParseError jerrormsg;
    QJsonDocument jmsg = QJsonDocument::fromJson(msg.payload(), &jerrormsg);
    if (jmsg.isNull()) {
        QJsonObject js;
        js["Error"] = jerrormsg.errorString();
        emit sendCommandUpdate(GetCommand(), js);
        qWarning() << "Json Parse Error for " << GetCommand() << ": " << jerrormsg.errorString() << ": " << msg.payload();
        return;     
    }
    QString field;
    foreach (field, this->m_requiredFields) {
        if (jmsg[field].isUndefined()) {
            QJsonObject js;
            js["Error"]  = QString("Missing Field ").append(field);
            emit sendCommandUpdate(GetCommand(), js);
            qWarning() << "Missing Field for " << GetCommand() << ": " << field << ": " << msg.payload();
            return;
        }
    }
    if (this->processMessage(jmsg)) {
        qInfo() << "Processed Message for " << GetCommand() << ": " << msg.payload();
        return;
    } else {
        qWarning() << "Message Processing for " << GetCommand() << " failed: " << msg.payload();
    }
}



MqttCommands::MqttCommands(QObject *parent) :
    QObject(parent)
{

}

void MqttCommands::Register(QString command, pfnCreateCommand_t _create) {
    qDebug() << "Registering Command " << command;
    this->m_commands.insert(command, _create);
}

void MqttCommands::setupCommands() {
    this->Register(MqttCommand_Ping::StaticGetCommand(), &MqttCommand_Ping::Create);
}

void MqttCommands::setupSubscriptions(QMqttClient *mqttclient, QString topTopic) {
    QMap<QString, pfnCreateCommand_t>::iterator it;
    for (it = this->m_commands.begin(); it != this->m_commands.end(); it++) {
        qDebug() << "Creating Subscription for " << it.key();
        pfnCreateCommand_t cmd = it.value();
        MqttCommand *command = cmd(this->parent());
        QMqttSubscription *subscription = mqttclient->subscribe(QMqttTopicFilter(topTopic.arg(it.key().toLower())));
        command->Setup(subscription);        
    }

    
}



