#include "mqttcommands/requestNodeNeighborUpdate.h"

MqttCommand_RequestNodeNeighborUpdate::MqttCommand_RequestNodeNeighborUpdate(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNodeNeighborUpdate::Create(QObject *parent) {
    return new MqttCommand_RequestNodeNeighborUpdate(parent);
}

bool MqttCommand_RequestNodeNeighborUpdate::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->requestNodeNeighborUpdate(msg["node"].GetUint()));
}