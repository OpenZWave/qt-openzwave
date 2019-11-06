#include "mqttcommands/requestNetworkUpdate.h"

MqttCommand_RequestNetworkUpdate::MqttCommand_RequestNetworkUpdate(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNetworkUpdate::Create(QObject *parent) {
    return new MqttCommand_RequestNetworkUpdate(parent);
}

bool MqttCommand_RequestNetworkUpdate::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    if (mgr->requestNetworkUpdate(msg["node"].toInt())) {
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