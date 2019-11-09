#include "mqttcommands/assignReturnRoute.h"

MqttCommand_AssignReturnRoute::MqttCommand_AssignReturnRoute(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_AssignReturnRoute::Create(QObject *parent) {
    return new MqttCommand_AssignReturnRoute(parent);
}

bool MqttCommand_AssignReturnRoute::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->assignReturnRoute(msg["node"].GetUint()));
}