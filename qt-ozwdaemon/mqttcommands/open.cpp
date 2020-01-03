#include "mqttcommands/open.h"

MqttCommand_Open::MqttCommand_Open(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredStringFields << "serialport";
}
MqttCommand* MqttCommand_Open::Create(QObject *parent) {
    return new MqttCommand_Open(parent);
}

bool MqttCommand_Open::processMessage(rapidjson::Document &msg) {
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->open(msg["serialport"].GetString()));
}