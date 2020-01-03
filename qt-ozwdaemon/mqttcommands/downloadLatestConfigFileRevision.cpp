#include "mqttcommands/downloadLatestConfigFileRevision.h"

MqttCommand_DownloadLatestConfigFileRevision::MqttCommand_DownloadLatestConfigFileRevision(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_DownloadLatestConfigFileRevision::Create(QObject *parent) {
    return new MqttCommand_DownloadLatestConfigFileRevision(parent);
}

bool MqttCommand_DownloadLatestConfigFileRevision::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->downloadLatestConfigFileRevision(msg["node"].GetUint()));
}