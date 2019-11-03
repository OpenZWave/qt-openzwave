#include "mqttcommands/addNodeSecure.h"

MqttCommand_AddNodeSecure::MqttCommand_AddNodeSecure(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_AddNodeSecure::Create(QObject *parent) {
    return new MqttCommand_AddNodeSecure(parent);
}

bool MqttCommand_AddNodeSecure::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    if (mgr->addNode(true)) {
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