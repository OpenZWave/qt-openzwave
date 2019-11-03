#include "mqttcommands/IsNodeFailed.h"

MqttCommand_IsNodeFailed::MqttCommand_IsNodeFailed(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_IsNodeFailed::Create(QObject *parent) {
    return new MqttCommand_IsNodeFailed(parent);
}

bool MqttCommand_IsNodeFailed::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->IsNodeFailed(msg["node"].toInt())) {
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