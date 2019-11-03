#include "mqttcommands/refreshnodeinfo.h"

MqttCommand_RefreshNodeInfo::MqttCommand_RefreshNodeInfo(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_RefreshNodeInfo::Create(QObject *parent) {
    return new MqttCommand_RefreshNodeInfo(parent);
}

bool MqttCommand_RefreshNodeInfo::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->refreshNodeInfo(msg["node"].toInt())) {
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