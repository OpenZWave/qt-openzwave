#include "mqttcommands/deleteAllReturnRoute.h"

MqttCommand_DeleteAllReturnRoute::MqttCommand_DeleteAllReturnRoute(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_DeleteAllReturnRoute::Create(QObject *parent) {
    return new MqttCommand_DeleteAllReturnRoute(parent);
}

bool MqttCommand_DeleteAllReturnRoute::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }

    QTOZWManager *mgr = getOZWManager();
    if (mgr->deleteAllReturnRoute(msg["node"].toInt())) {
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