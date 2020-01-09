#include "mqttcommands/removeAssociation.h"

MqttCommand_RemoveAssociation::MqttCommand_RemoveAssociation(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node" << "group";
    this->m_requiredStringFields << "target";
}
MqttCommand* MqttCommand_RemoveAssociation::Create(QObject *parent) {
    return new MqttCommand_RemoveAssociation(parent);
}

bool MqttCommand_RemoveAssociation::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->RemoveAssociation(msg["node"].GetUint(), msg["group"].GetUint(), msg["target"].GetString()));
}