#include "mqttcommands/testNetworkNode.h"

MqttCommand_TestNetworkNode::MqttCommand_TestNetworkNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node" << "count";
}
MqttCommand* MqttCommand_TestNetworkNode::Create(QObject *parent) {
    return new MqttCommand_TestNetworkNode(parent);
}

bool MqttCommand_TestNetworkNode::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->testNetworkNode(msg["node"].toInt(), msg["count"].toInt());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}