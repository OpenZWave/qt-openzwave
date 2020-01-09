#include "mqttcommands/addAssociation.h"

MqttCommand_AddAssociation::MqttCommand_AddAssociation(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node" << "group";
    this->m_requiredStringFields << "target";
}
MqttCommand* MqttCommand_AddAssociation::Create(QObject *parent) {
    return new MqttCommand_AddAssociation(parent);
}

bool MqttCommand_AddAssociation::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->AddAssociation(msg["node"].GetUint(), msg["group"].GetUint(), msg["target"].GetString()));
}