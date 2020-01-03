#include "mqttcommands/softResetController.h"

MqttCommand_SoftResetController::MqttCommand_SoftResetController(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_SoftResetController::Create(QObject *parent) {
    return new MqttCommand_SoftResetController(parent);
}

bool MqttCommand_SoftResetController::processMessage(rapidjson::Document &msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    mgr->softResetController();
    return this->sendSimpleStatus(true);
}