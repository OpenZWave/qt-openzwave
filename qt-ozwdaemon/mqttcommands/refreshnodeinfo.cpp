#include "mqttcommands/refreshnodeinfo.h"

MqttCommand_RefreshNodeInfo::MqttCommand_RefreshNodeInfo(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RefreshNodeInfo::Create(QObject *parent) {
    return new MqttCommand_RefreshNodeInfo(parent);
}

bool MqttCommand_RefreshNodeInfo::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
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