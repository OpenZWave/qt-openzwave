#include "mqttcommands/ping.h"

MqttCommand_Ping::MqttCommand_Ping(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredStringFields << "ping";
}
MqttCommand* MqttCommand_Ping::Create(QObject *parent) {
    return new MqttCommand_Ping(parent);
}

bool MqttCommand_Ping::processMessage(QJsonDocument msg) {
    QJsonObject js;
    js["pong"] = msg["ping"];
    emit sendCommandUpdate(MqttCommand_Ping::GetCommand(), js);
    return true;
}