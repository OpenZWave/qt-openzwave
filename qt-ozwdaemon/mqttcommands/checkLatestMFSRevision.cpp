#include "mqttcommands/checkLatestMFSRevision.h"

MqttCommand_CheckLatestMFSRevision::MqttCommand_CheckLatestMFSRevision(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_CheckLatestMFSRevision::Create(QObject *parent) {
    return new MqttCommand_CheckLatestMFSRevision(parent);
}

bool MqttCommand_CheckLatestMFSRevision::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    mgr->checkLatestMFSRevision();
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}