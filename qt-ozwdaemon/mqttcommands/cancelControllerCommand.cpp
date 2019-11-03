#include "mqttcommands/cancelControllerCommand.h"

MqttCommand_CancelControllerCommand::MqttCommand_CancelControllerCommand(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_CancelControllerCommand::Create(QObject *parent) {
    return new MqttCommand_CancelControllerCommand(parent);
}

bool MqttCommand_CancelControllerCommand::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    if (mgr->cancelControllerCommand()) {
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