#include "mqttcommands/downloadLatestMFSRevision.h"

MqttCommand_DownloadLatestMFSRevision::MqttCommand_DownloadLatestMFSRevision(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_DownloadLatestMFSRevision::Create(QObject *parent) {
    return new MqttCommand_DownloadLatestMFSRevision(parent);
}

bool MqttCommand_DownloadLatestMFSRevision::processMessage(rapidjson::Document &msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->downloadLatestMFSRevision());
}