#include "mqttcommands/healNetworkNode.h"

MqttCommand_HealNetworkNode::MqttCommand_HealNetworkNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
    this->m_requiredBoolFields << "doreturnroute";
}
MqttCommand* MqttCommand_HealNetworkNode::Create(QObject *parent) {
    return new MqttCommand_HealNetworkNode(parent);
}

bool MqttCommand_HealNetworkNode::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    mgr->healNetworkNode(msg["node"].toInt(), msg["doreturnroute"].toBool());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}