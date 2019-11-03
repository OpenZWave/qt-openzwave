#include "mqttcommands/checkLatestConfigFileRevision.h"

MqttCommand_CheckLatestConfigFileRevision::MqttCommand_CheckLatestConfigFileRevision(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_CheckLatestConfigFileRevision::Create(QObject *parent) {
    return new MqttCommand_CheckLatestConfigFileRevision(parent);
}

bool MqttCommand_CheckLatestConfigFileRevision::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->checkLatestConfigFileRevision(msg["node"].toInt())) {
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