#include "mqttcommands/requestNodeDynamic.h"

MqttCommand_RequestNodeDynamic::MqttCommand_RequestNodeDynamic(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_RequestNodeDynamic::Create(QObject *parent) {
    return new MqttCommand_RequestNodeDynamic(parent);
}

bool MqttCommand_RequestNodeDynamic::processMessage(QJsonDocument msg) {
    if (!this->checkNode(msg, "node")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid Node Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZWManager *mgr = getOZWManager();
    if (mgr->requestNodeDynamic(msg["node"].toInt())) {
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