#include "mqttcommands/sendNodeInformation.h"

MqttCommand_SendNodeInformation::MqttCommand_SendNodeInformation(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_SendNodeInformation::Create(QObject *parent) {
    return new MqttCommand_SendNodeInformation(parent);
}

bool MqttCommand_SendNodeInformation::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    if (mgr->sendNodeInformation(msg["node"].toInt())) {
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