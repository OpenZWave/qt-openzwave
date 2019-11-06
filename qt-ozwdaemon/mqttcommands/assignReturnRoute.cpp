#include "mqttcommands/assignReturnRoute.h"

MqttCommand_AssignReturnRoute::MqttCommand_AssignReturnRoute(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_AssignReturnRoute::Create(QObject *parent) {
    return new MqttCommand_AssignReturnRoute(parent);
}

bool MqttCommand_AssignReturnRoute::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    if (mgr->assignReturnRoute(msg["node"].toInt())) {
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