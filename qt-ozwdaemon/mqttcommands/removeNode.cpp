#include "mqttcommands/removeNode.h"

MqttCommand_RemoveNode::MqttCommand_RemoveNode(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_RemoveNode::Create(QObject *parent) {
    return new MqttCommand_RemoveNode(parent);
}

bool MqttCommand_RemoveNode::processMessage(rapidjson::Document &msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    mgr->removeNode();
    return this->sendSimpleStatus(true);
}