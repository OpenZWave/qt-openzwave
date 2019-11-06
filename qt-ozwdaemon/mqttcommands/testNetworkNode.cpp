#include "mqttcommands/testNetworkNode.h"

MqttCommand_TestNetworkNode::MqttCommand_TestNetworkNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node" << "count";
}
MqttCommand* MqttCommand_TestNetworkNode::Create(QObject *parent) {
    return new MqttCommand_TestNetworkNode(parent);
}

bool MqttCommand_TestNetworkNode::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    mgr->testNetworkNode(msg["node"].toInt(), msg["count"].toInt());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}