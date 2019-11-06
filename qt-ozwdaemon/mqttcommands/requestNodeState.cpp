#include "mqttcommands/requestNodeState.h"

MqttCommand_RequestNodeState::MqttCommand_RequestNodeState(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNodeState::Create(QObject *parent) {
    return new MqttCommand_RequestNodeState(parent);
}

bool MqttCommand_RequestNodeState::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
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