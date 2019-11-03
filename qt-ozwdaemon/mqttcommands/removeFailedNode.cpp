#include "mqttcommands/removeFailedNode.h"

MqttCommand_RemoveFailedNode::MqttCommand_RemoveFailedNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_RemoveFailedNode::Create(QObject *parent) {
    return new MqttCommand_RemoveFailedNode(parent);
}

bool MqttCommand_RemoveFailedNode::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->removeFailedNode(msg["node"].toInt())) {
        QJsonObject js;
        js["status"] = "ok";
        emit sendCommandUpdate(GetCommand(), js);
        return true;
    }
    QJsonObject js;
    js["status"] = "failed";
    emit sendCommandUpdate(GetCommand(), js);
    return false;
}