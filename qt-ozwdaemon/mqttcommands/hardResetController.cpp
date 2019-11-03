#include "mqttcommands/hardResetController.h"

MqttCommand_HardResetController::MqttCommand_HardResetController(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_HardResetController::Create(QObject *parent) {
    return new MqttCommand_HardResetController(parent);
}

bool MqttCommand_HardResetController::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    mgr->hardResetController();
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}