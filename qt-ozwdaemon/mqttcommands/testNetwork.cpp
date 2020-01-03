#include "mqttcommands/testNetwork.h"

MqttCommand_TestNetwork::MqttCommand_TestNetwork(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "count";
}
MqttCommand* MqttCommand_TestNetwork::Create(QObject *parent) {
    return new MqttCommand_TestNetwork(parent);
}

bool MqttCommand_TestNetwork::processMessage(rapidjson::Document &msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->testNetwork(msg["count"].GetUint());
    return this->sendSimpleStatus(true);
}