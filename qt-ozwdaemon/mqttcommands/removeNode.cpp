#include "mqttcommands/removeNode.h"

MqttCommand_RemoveNode::MqttCommand_RemoveNode(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_RemoveNode::Create(QObject *parent) {
    return new MqttCommand_RemoveNode(parent);
}

bool MqttCommand_RemoveNode::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    mgr->removeNode();
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}