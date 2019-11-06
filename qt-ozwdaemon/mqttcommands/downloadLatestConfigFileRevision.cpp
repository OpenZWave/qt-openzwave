#include "mqttcommands/downloadLatestConfigFileRevision.h"

MqttCommand_DownloadLatestConfigFileRevision::MqttCommand_DownloadLatestConfigFileRevision(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_DownloadLatestConfigFileRevision::Create(QObject *parent) {
    return new MqttCommand_DownloadLatestConfigFileRevision(parent);
}

bool MqttCommand_DownloadLatestConfigFileRevision::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    if (mgr->downloadLatestConfigFileRevision(msg["node"].toInt())) {
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