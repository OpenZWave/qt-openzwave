#include "mqttcommands/requestAllConfigParam.h"

MqttCommand_RequestAllConfigParam::MqttCommand_RequestAllConfigParam(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_RequestAllConfigParam::Create(QObject *parent) {
    return new MqttCommand_RequestAllConfigParam(parent);
}

bool MqttCommand_RequestAllConfigParam::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->requestAllConfigParam(msg["node"].toInt());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}