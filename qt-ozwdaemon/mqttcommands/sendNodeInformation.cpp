#include "mqttcommands/sendNodeInformation.h"

MqttCommand_SendNodeInformation::MqttCommand_SendNodeInformation(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_SendNodeInformation::Create(QObject *parent) {
    return new MqttCommand_SendNodeInformation(parent);
}

bool MqttCommand_SendNodeInformation::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->sendNodeInformation(msg["node"].GetUint()));
}