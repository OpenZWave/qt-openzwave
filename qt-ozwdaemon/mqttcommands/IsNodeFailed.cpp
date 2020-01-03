#include "mqttcommands/IsNodeFailed.h"

MqttCommand_IsNodeFailed::MqttCommand_IsNodeFailed(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_IsNodeFailed::Create(QObject *parent) {
    return new MqttCommand_IsNodeFailed(parent);
}

bool MqttCommand_IsNodeFailed::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->IsNodeFailed(msg["node"].GetUint()));
}