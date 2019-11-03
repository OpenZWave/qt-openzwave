#include "mqttcommands/healNetworkNode.h"

MqttCommand_HealNetworkNode::MqttCommand_HealNetworkNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node" << "doreturnroute";
}
MqttCommand* MqttCommand_HealNetworkNode::Create(QObject *parent) {
    return new MqttCommand_HealNetworkNode(parent);
}

bool MqttCommand_HealNetworkNode::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->healNetworkNode(msg["node"].toInt(), msg["doreturnroute"].toBool());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}