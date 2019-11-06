#include "mqttcommands/healNetwork.h"

MqttCommand_HealNetwork::MqttCommand_HealNetwork(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredBoolFields << "doreturnroute";
}
MqttCommand* MqttCommand_HealNetwork::Create(QObject *parent) {
    return new MqttCommand_HealNetwork(parent);
}

bool MqttCommand_HealNetwork::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->healNetwork(msg["doreturnroute"].toBool());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}