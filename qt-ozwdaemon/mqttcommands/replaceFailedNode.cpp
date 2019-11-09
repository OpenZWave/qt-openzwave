#include "mqttcommands/replaceFailedNode.h"

MqttCommand_ReplaceFailedNode::MqttCommand_ReplaceFailedNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_ReplaceFailedNode::Create(QObject *parent) {
    return new MqttCommand_ReplaceFailedNode(parent);
}

bool MqttCommand_ReplaceFailedNode::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->replaceFailedNode(msg["node"].GetUint()));
}