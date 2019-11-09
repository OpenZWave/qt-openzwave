#include "mqttcommands/healNetwork.h"

MqttCommand_HealNetwork::MqttCommand_HealNetwork(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredBoolFields << "doreturnroute";
}
MqttCommand* MqttCommand_HealNetwork::Create(QObject *parent) {
    return new MqttCommand_HealNetwork(parent);
}

bool MqttCommand_HealNetwork::processMessage(rapidjson::Document &msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->healNetwork(msg["doreturnroute"].GetBool());
    return this->sendSimpleStatus(true);
}