#include "mqttcommands/requestNodeState.h"

MqttCommand_RequestNodeState::MqttCommand_RequestNodeState(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNodeState::Create(QObject *parent) {
    return new MqttCommand_RequestNodeState(parent);
}

bool MqttCommand_RequestNodeState::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->requestNodeState(msg["node"].GetUint()));
}