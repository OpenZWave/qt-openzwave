#include "mqttcommands/addNode.h"

MqttCommand_AddNode::MqttCommand_AddNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredBoolFields << "secure";
}
MqttCommand* MqttCommand_AddNode::Create(QObject *parent) {
    return new MqttCommand_AddNode(parent);
}

bool MqttCommand_AddNode::processMessage(rapidjson::Document &msg) {
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->addNode(msg["secure"].GetBool()));
}