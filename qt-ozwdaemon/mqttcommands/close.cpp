#include "mqttcommands/close.h"

MqttCommand_Close::MqttCommand_Close(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_Close::Create(QObject *parent) {
    return new MqttCommand_Close(parent);
}

bool MqttCommand_Close::processMessage(rapidjson::Document &msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->close());
}