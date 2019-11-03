#include "mqttcommands/requestNodeState.h"

MqttCommand_RequestNodeState::MqttCommand_RequestNodeState(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_RequestNodeState::Create(QObject *parent) {
    return new MqttCommand_RequestNodeState(parent);
}

bool MqttCommand_RequestNodeState::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->requestNodeState(msg["node"].toInt())) {
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