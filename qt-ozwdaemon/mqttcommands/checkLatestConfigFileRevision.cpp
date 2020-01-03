#include "mqttcommands/checkLatestConfigFileRevision.h"

MqttCommand_CheckLatestConfigFileRevision::MqttCommand_CheckLatestConfigFileRevision(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_CheckLatestConfigFileRevision::Create(QObject *parent) {
    return new MqttCommand_CheckLatestConfigFileRevision(parent);
}

bool MqttCommand_CheckLatestConfigFileRevision::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }

    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->checkLatestConfigFileRevision(msg["node"].GetUint()));
}