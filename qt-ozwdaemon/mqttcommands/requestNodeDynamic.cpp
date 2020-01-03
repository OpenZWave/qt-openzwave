#include "mqttcommands/requestNodeDynamic.h"

MqttCommand_RequestNodeDynamic::MqttCommand_RequestNodeDynamic(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNodeDynamic::Create(QObject *parent) {
    return new MqttCommand_RequestNodeDynamic(parent);
}

bool MqttCommand_RequestNodeDynamic::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->requestNodeDynamic(msg["node"].GetUint()));
}