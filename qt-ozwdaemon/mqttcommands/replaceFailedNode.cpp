#include "mqttcommands/replaceFailedNode.h"

MqttCommand_ReplaceFailedNode::MqttCommand_ReplaceFailedNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_ReplaceFailedNode::Create(QObject *parent) {
    return new MqttCommand_ReplaceFailedNode(parent);
}

bool MqttCommand_ReplaceFailedNode::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->replaceFailedNode(msg["node"].toInt())) {
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