#include "mqttcommands/healNetworkNode.h"

MqttCommand_HealNetworkNode::MqttCommand_HealNetworkNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
    this->m_requiredBoolFields << "doreturnroute";
}
MqttCommand* MqttCommand_HealNetworkNode::Create(QObject *parent) {
    return new MqttCommand_HealNetworkNode(parent);
}

bool MqttCommand_HealNetworkNode::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    mgr->healNetworkNode(msg["node"].GetUint(), msg["doreturnroute"].GetBool());
    return this->sendSimpleStatus(true);
}