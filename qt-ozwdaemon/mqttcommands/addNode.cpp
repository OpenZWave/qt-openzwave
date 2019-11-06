#include "mqttcommands/addNode.h"

MqttCommand_AddNode::MqttCommand_AddNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredBoolFields << "secure";
}
MqttCommand* MqttCommand_AddNode::Create(QObject *parent) {
    return new MqttCommand_AddNode(parent);
}

bool MqttCommand_AddNode::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    if (mgr->addNode(msg["secure"].toBool())) {
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