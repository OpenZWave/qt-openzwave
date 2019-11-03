#include "mqttcommands/testNetwork.h"

MqttCommand_TestNetwork::MqttCommand_TestNetwork(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "count";
}
MqttCommand* MqttCommand_TestNetwork::Create(QObject *parent) {
    return new MqttCommand_TestNetwork(parent);
}

bool MqttCommand_TestNetwork::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->testNetwork(msg["count"].toInt());
    QJsonObject js;
    js["status"] = "ok";
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}