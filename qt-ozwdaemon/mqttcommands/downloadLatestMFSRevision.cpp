#include "mqttcommands/downloadLatestMFSRevision.h"

MqttCommand_DownloadLatestMFSRevision::MqttCommand_DownloadLatestMFSRevision(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_DownloadLatestMFSRevision::Create(QObject *parent) {
    return new MqttCommand_DownloadLatestMFSRevision(parent);
}

bool MqttCommand_DownloadLatestMFSRevision::processMessage(QJsonDocument msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    if (mgr->downloadLatestMFSRevision()) {
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