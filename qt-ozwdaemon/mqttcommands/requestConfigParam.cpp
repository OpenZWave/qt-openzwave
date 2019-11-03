#include "mqttcommands/requestConfigParam.h"

MqttCommand_RequestConfigParam::MqttCommand_RequestConfigParam(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node" << "param";
}
MqttCommand* MqttCommand_RequestConfigParam::Create(QObject *parent) {
    return new MqttCommand_RequestConfigParam(parent);
}

bool MqttCommand_RequestConfigParam::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->requestConfigParam(msg["node"].toInt(), msg["param"].toInt());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}