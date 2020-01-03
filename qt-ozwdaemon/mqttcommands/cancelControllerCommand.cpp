#include "mqttcommands/cancelControllerCommand.h"

MqttCommand_CancelControllerCommand::MqttCommand_CancelControllerCommand(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_CancelControllerCommand::Create(QObject *parent) {
    return new MqttCommand_CancelControllerCommand(parent);
}

bool MqttCommand_CancelControllerCommand::processMessage(rapidjson::Document &msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->cancelControllerCommand());
}