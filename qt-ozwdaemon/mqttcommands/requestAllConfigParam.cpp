#include "mqttcommands/requestAllConfigParam.h"

MqttCommand_RequestAllConfigParam::MqttCommand_RequestAllConfigParam(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestAllConfigParam::Create(QObject *parent) {
    return new MqttCommand_RequestAllConfigParam(parent);
}

bool MqttCommand_RequestAllConfigParam::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    mgr->requestAllConfigParam(msg["node"].GetUint());
    return this->sendSimpleStatus(true);
}