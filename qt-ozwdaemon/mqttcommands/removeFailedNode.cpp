#include "mqttcommands/removeFailedNode.h"

MqttCommand_RemoveFailedNode::MqttCommand_RemoveFailedNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RemoveFailedNode::Create(QObject *parent) {
    return new MqttCommand_RemoveFailedNode(parent);
}

bool MqttCommand_RemoveFailedNode::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->removeFailedNode(msg["node"].GetUint()));
}