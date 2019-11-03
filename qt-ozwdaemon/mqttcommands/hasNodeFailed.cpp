#include "mqttcommands/hasNodeFailed.h"

MqttCommand_HasNodeFailed::MqttCommand_HasNodeFailed(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_HasNodeFailed::Create(QObject *parent) {
    return new MqttCommand_HasNodeFailed(parent);
}

bool MqttCommand_HasNodeFailed::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->hasNodeFailed(msg["node"].toInt())) {
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