#include "mqttcommands/downloadLatestConfigFileRevision.h"

MqttCommand_DownloadLatestConfigFileRevision::MqttCommand_DownloadLatestConfigFileRevision(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_DownloadLatestConfigFileRevision::Create(QObject *parent) {
    return new MqttCommand_DownloadLatestConfigFileRevision(parent);
}

bool MqttCommand_DownloadLatestConfigFileRevision::processMessage(QJsonDocument msg) {
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