#include "mqttcommands/requestNodeNeighborUpdate.h"

MqttCommand_RequestNodeNeighborUpdate::MqttCommand_RequestNodeNeighborUpdate(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNodeNeighborUpdate::Create(QObject *parent) {
    return new MqttCommand_RequestNodeNeighborUpdate(parent);
}

bool MqttCommand_RequestNodeNeighborUpdate::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    if (mgr->requestNodeNeighborUpdate(msg["node"].toInt())) {
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