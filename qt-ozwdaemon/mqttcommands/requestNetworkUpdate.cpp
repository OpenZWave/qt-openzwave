#include "mqttcommands/requestNetworkUpdate.h"

MqttCommand_RequestNetworkUpdate::MqttCommand_RequestNetworkUpdate(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNetworkUpdate::Create(QObject *parent) {
    return new MqttCommand_RequestNetworkUpdate(parent);
}

bool MqttCommand_RequestNetworkUpdate::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->requestNetworkUpdate(msg["node"].GetUint()));
}