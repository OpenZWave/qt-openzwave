#include "mqttcommands/ping.h"

MqttCommand_Ping::MqttCommand_Ping(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredStringFields << "ping";
}
MqttCommand* MqttCommand_Ping::Create(QObject *parent) {
    return new MqttCommand_Ping(parent);
}

bool MqttCommand_Ping::processMessage(rapidjson::Document &msg) {
    rapidjson::Document js;
    QT2JS::SetString(js, "pong", msg["ping"].GetString());
    emit sendCommandUpdate(MqttCommand_Ping::GetCommand(), js);
    return true;
}