#include "mqttcommands/refreshnodeinfo.h"

MqttCommand_RefreshNodeInfo::MqttCommand_RefreshNodeInfo(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RefreshNodeInfo::Create(QObject *parent) {
    return new MqttCommand_RefreshNodeInfo(parent);
}

bool MqttCommand_RefreshNodeInfo::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->refreshNodeInfo(msg["node"].GetUint()));
}