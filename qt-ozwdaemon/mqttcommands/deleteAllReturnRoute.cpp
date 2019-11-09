#include "mqttcommands/deleteAllReturnRoute.h"

MqttCommand_DeleteAllReturnRoute::MqttCommand_DeleteAllReturnRoute(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_DeleteAllReturnRoute::Create(QObject *parent) {
    return new MqttCommand_DeleteAllReturnRoute(parent);
}

bool MqttCommand_DeleteAllReturnRoute::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        this->sendSimpleStatus(false, "Invalid Node Number");
    }

    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->deleteAllReturnRoute(msg["node"].GetUint()));
}