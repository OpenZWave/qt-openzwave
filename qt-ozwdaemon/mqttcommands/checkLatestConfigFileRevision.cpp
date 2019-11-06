#include "mqttcommands/checkLatestConfigFileRevision.h"

MqttCommand_CheckLatestConfigFileRevision::MqttCommand_CheckLatestConfigFileRevision(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_CheckLatestConfigFileRevision::Create(QObject *parent) {
    return new MqttCommand_CheckLatestConfigFileRevision(parent);
}

bool MqttCommand_CheckLatestConfigFileRevision::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }

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