#include "mqttcommands/mqttcommands.h"
#include "mqttcommands/ping.h"
#include "mqttcommands/open.h"
#include "mqttcommands/refreshnodeinfo.h"
#include "mqttcommands/requestNodeState.h"
#include "mqttcommands/requestNodeDynamic.h"
#include "mqttcommands/requestConfigParam.h"
#include "mqttcommands/requestAllConfigParam.h"
#include "mqttcommands/softResetController.h"
#include "mqttcommands/hardResetController.h"
#include "mqttcommands/cancelControllerCommand.h"
#include "mqttcommands/testNetworkNode.h"
#include "mqttcommands/testNetwork.h"
#include "mqttcommands/healNetworkNode.h"
#include "mqttcommands/healNetwork.h"
#include "mqttcommands/addNode.h"
#include "mqttcommands/removeNode.h"
#include "mqttcommands/removeFailedNode.h"
#include "mqttcommands/hasNodeFailed.h"
#include "mqttcommands/requestNodeNeighborUpdate.h"
#include "mqttcommands/assignReturnRoute.h"
#include "mqttcommands/deleteAllReturnRoute.h"
#include "mqttcommands/sendNodeInformation.h"
#include "mqttcommands/replaceFailedNode.h"
#include "mqttcommands/requestNetworkUpdate.h"
#include "mqttcommands/IsNodeFailed.h"
#include "mqttcommands/checkLatestConfigFileRevision.h"
#include "mqttcommands/checkLatestMFSRevision.h"
#include "mqttcommands/downloadLatestConfigFileRevision.h"
#include "mqttcommands/downloadLatestMFSRevision.h"
#include "mqttcommands/setValue.h"

Q_LOGGING_CATEGORY(ozwmc, "ozw.mqtt.commands");

MqttCommand::MqttCommand(QObject *parent) :
    QObject(parent)
{
}

void MqttCommand::Setup(QMqttSubscription *subscription) {
    this->m_subscription = subscription;
    qCDebug(ozwmc) << "Subscription Setup for " << this->m_subscription->topic();
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
    qCDebug(ozwmc) << "Got "<< msg.topic().name()<< " Message: " << msg.payload();
    QJsonParseError jerrormsg;
    QJsonDocument jmsg = QJsonDocument::fromJson(msg.payload(), &jerrormsg);
    if (jmsg.isNull()) {
        QJsonObject js;
        js["Error"] = jerrormsg.errorString();
        emit sendCommandUpdate(GetCommand(), js);
        qCWarning(ozwmc) << "Json Parse Error for " << GetCommand() << ": " << jerrormsg.errorString() << ": " << msg.payload();
        return;     
    }
    QString field;
    foreach (field, this->m_requiredIntFields) {
        if (jmsg[field].isUndefined()) {
            QJsonObject js;
            js["Error"]  = QString("Missing Field ").append(field);
            emit sendCommandUpdate(GetCommand(), js);
            qCWarning(ozwmc) << "Missing Field for " << GetCommand() << ": " << field << ": " << msg.payload();
            return;
        }
        if (!jmsg[field].isDouble()) {
            QJsonObject js;
            js["Error"]  = QString("Incorrect Field Type: ").append(field).append(": Not Integer");
            emit sendCommandUpdate(GetCommand(), js);
            qCWarning(ozwmc) << "Incorrect Field Type (Int) for " << GetCommand() << ": " << field << ": " << jmsg[field].type() << msg.payload();
            return;
        }
    }
    foreach (field, this->m_requiredStringFields) {
        if (jmsg[field].isUndefined()) {
            QJsonObject js;
            js["Error"]  = QString("Missing Field ").append(field);
            emit sendCommandUpdate(GetCommand(), js);
            qCWarning(ozwmc) << "Missing Field for " << GetCommand() << ": " << field << ": " << msg.payload();
            return;
        }
        if (!jmsg[field].isString()) {
            QJsonObject js;
            js["Error"]  = QString("Incorrect Field Type: ").append(field).append(": Not String");
            emit sendCommandUpdate(GetCommand(), js);
            qCWarning(ozwmc) << "Incorrect Field Type (String) for " << GetCommand() << ": " << field << ": " << jmsg[field].type() << msg.payload();
            return;
        }
    }
    foreach (field, this->m_requiredBoolFields) {
        if (jmsg[field].isUndefined()) {
            QJsonObject js;
            js["Error"]  = QString("Missing Field ").append(field);
            emit sendCommandUpdate(GetCommand(), js);
            qCWarning(ozwmc) << "Missing Field for " << GetCommand() << ": " << field << ": " << msg.payload();
            return;
        }
        if (!jmsg[field].isBool()) {
            QJsonObject js;
            js["Error"]  = QString("Incorrect Field Type: ").append(field).append(": Not Bool");
            emit sendCommandUpdate(GetCommand(), js);
            qCWarning(ozwmc) << "Incorrect Field Type (Bool) for " << GetCommand() << ": " << field << ": " << jmsg[field].type() << msg.payload();
            return;
        }
    }

    if (this->processMessage(jmsg)) {
        qCInfo(ozwmc) << "Processed Message for " << GetCommand() << ": " << msg.payload();
        return;
    } else {
        qCWarning(ozwmc) << "Message Processing for " << GetCommand() << " failed: " << msg.payload();
    }
}

bool MqttCommand::checkNode(QJsonDocument jmsg, QString field) {
    quint8 node = jmsg[field].toInt();
    if (node == 0 || node == 255) {
        qCWarning(ozwmc) << "Invalid Node in field " << field << " for message " << jmsg.toJson();
        return false;
    }
    if (this->getMqttPublisher()->isValidNode(node)) {
        return true;
    }
    qCWarning(ozwmc) << "Invalid Node in field " << field << " for message " << jmsg.toJson();
    return false;
}

bool MqttCommand::checkValue(QJsonDocument jmsg, QString field) {
    quint64 vidKey = jmsg[field].toInt();
    if (vidKey == 0) {
        qCWarning(ozwmc) << "Invalid VidKey in field " << field << " for message " << jmsg.toJson();
        return false;
    }
    if (this->getMqttPublisher()->isValidValueID(vidKey)) {
        return true;
    }
    qCWarning(ozwmc) << "Invalid VidKey in field " << field << " for message " << jmsg.toJson();
    return false;
}

QVariant MqttCommand::getValueData(quint64 vidKey, QTOZW_ValueIds::ValueIdColumns col) {
    return this->getMqttPublisher()->getValueData(vidKey, col);
}

bool MqttCommand::setValue(quint64 vidKey, QVariant data) {
    return this->getMqttPublisher()->setValue(vidKey, data);
}


MqttCommands::MqttCommands(QObject *parent) :
    QObject(parent)
{

}

void MqttCommands::Register(QString command, pfnCreateCommand_t _create) {
    qCDebug(ozwmc) << "Registering Command " << command;
    this->m_commands.insert(command, _create);
}

void MqttCommands::setupCommands() {
    this->Register(MqttCommand_Ping::StaticGetCommand(), &MqttCommand_Ping::Create);
    this->Register(MqttCommand_Open::StaticGetCommand(), &MqttCommand_Open::Create);
    this->Register(MqttCommand_RefreshNodeInfo::StaticGetCommand(), &MqttCommand_RefreshNodeInfo::Create);
    this->Register(MqttCommand_RequestNodeState::StaticGetCommand(), &MqttCommand_RequestNodeState::Create);
    this->Register(MqttCommand_RequestNodeDynamic::StaticGetCommand(), &MqttCommand_RequestNodeDynamic::Create);
    this->Register(MqttCommand_RequestConfigParam::StaticGetCommand(), &MqttCommand_RequestConfigParam::Create);
    this->Register(MqttCommand_RequestAllConfigParam::StaticGetCommand(), &MqttCommand_RequestAllConfigParam::Create);
    this->Register(MqttCommand_SoftResetController::StaticGetCommand(), &MqttCommand_SoftResetController::Create);
    this->Register(MqttCommand_HardResetController::StaticGetCommand(), &MqttCommand_HardResetController::Create);
    this->Register(MqttCommand_CancelControllerCommand::StaticGetCommand(), &MqttCommand_CancelControllerCommand::Create);
    this->Register(MqttCommand_TestNetworkNode::StaticGetCommand(), &MqttCommand_TestNetworkNode::Create);
    this->Register(MqttCommand_TestNetwork::StaticGetCommand(), &MqttCommand_TestNetwork::Create);
    this->Register(MqttCommand_HealNetworkNode::StaticGetCommand(), &MqttCommand_HealNetworkNode::Create);
    this->Register(MqttCommand_HealNetwork::StaticGetCommand(), &MqttCommand_HealNetwork::Create);
    this->Register(MqttCommand_AddNode::StaticGetCommand(), &MqttCommand_AddNode::Create);
    this->Register(MqttCommand_RemoveNode::StaticGetCommand(), &MqttCommand_RemoveNode::Create);
    this->Register(MqttCommand_RemoveFailedNode::StaticGetCommand(), &MqttCommand_RemoveFailedNode::Create);
    this->Register(MqttCommand_HasNodeFailed::StaticGetCommand(), &MqttCommand_HasNodeFailed::Create);
    this->Register(MqttCommand_RequestNodeNeighborUpdate::StaticGetCommand(), &MqttCommand_RequestNodeNeighborUpdate::Create);
    this->Register(MqttCommand_AssignReturnRoute::StaticGetCommand(), &MqttCommand_AssignReturnRoute::Create);
    this->Register(MqttCommand_DeleteAllReturnRoute::StaticGetCommand(), &MqttCommand_DeleteAllReturnRoute::Create);
    this->Register(MqttCommand_SendNodeInformation::StaticGetCommand(), &MqttCommand_SendNodeInformation::Create);
    this->Register(MqttCommand_ReplaceFailedNode::StaticGetCommand(), &MqttCommand_ReplaceFailedNode::Create);
    this->Register(MqttCommand_RequestNetworkUpdate::StaticGetCommand(), &MqttCommand_RequestNetworkUpdate::Create);
    this->Register(MqttCommand_IsNodeFailed::StaticGetCommand(), &MqttCommand_IsNodeFailed::Create);
    this->Register(MqttCommand_CheckLatestConfigFileRevision::StaticGetCommand(), &MqttCommand_CheckLatestConfigFileRevision::Create);
    this->Register(MqttCommand_CheckLatestMFSRevision::StaticGetCommand(), &MqttCommand_CheckLatestMFSRevision::Create);
    this->Register(MqttCommand_DownloadLatestConfigFileRevision::StaticGetCommand(), &MqttCommand_DownloadLatestConfigFileRevision::Create);
    this->Register(MqttCommand_DownloadLatestMFSRevision::StaticGetCommand(), &MqttCommand_DownloadLatestMFSRevision::Create);
    this->Register(MqttCommand_SetValue::StaticGetCommand(), &MqttCommand_SetValue::Create);
}

void MqttCommands::setupSubscriptions(QMqttClient *mqttclient, QString topTopic) {
    QMap<QString, pfnCreateCommand_t>::iterator it;
    for (it = this->m_commands.begin(); it != this->m_commands.end(); it++) {
        qCDebug(ozwmc) << "Creating Subscription for " << it.key();
        pfnCreateCommand_t cmd = it.value();
        MqttCommand *command = cmd(this->parent());
        QMqttSubscription *subscription = mqttclient->subscribe(QMqttTopicFilter(topTopic.arg(it.key().toLower())));
        command->Setup(subscription);        
    }

    
}



