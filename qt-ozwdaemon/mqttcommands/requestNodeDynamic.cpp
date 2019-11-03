#include "mqttcommands/requestNodeDynamic.h"

MqttCommand_RequestNodeDynamic::MqttCommand_RequestNodeDynamic(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_RequestNodeDynamic::Create(QObject *parent) {
    return new MqttCommand_RequestNodeDynamic(parent);
}

bool MqttCommand_RequestNodeDynamic::processMessage(QJsonDocument msg) {
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