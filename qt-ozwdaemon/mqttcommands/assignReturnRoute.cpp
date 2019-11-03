#include "mqttcommands/assignReturnRoute.h"

MqttCommand_AssignReturnRoute::MqttCommand_AssignReturnRoute(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_AssignReturnRoute::Create(QObject *parent) {
    return new MqttCommand_AssignReturnRoute(parent);
}

bool MqttCommand_AssignReturnRoute::processMessage(QJsonDocument msg) {
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