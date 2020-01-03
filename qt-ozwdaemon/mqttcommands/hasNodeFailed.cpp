#include "mqttcommands/hasNodeFailed.h"

MqttCommand_HasNodeFailed::MqttCommand_HasNodeFailed(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_HasNodeFailed::Create(QObject *parent) {
    return new MqttCommand_HasNodeFailed(parent);
}

bool MqttCommand_HasNodeFailed::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->hasNodeFailed(msg["node"].GetUint()));
}