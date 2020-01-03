#include "mqttcommands/testNetworkNode.h"

MqttCommand_TestNetworkNode::MqttCommand_TestNetworkNode(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node" << "count";
}
MqttCommand* MqttCommand_TestNetworkNode::Create(QObject *parent) {
    return new MqttCommand_TestNetworkNode(parent);
}

bool MqttCommand_TestNetworkNode::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    mgr->testNetworkNode(msg["node"].GetUint64(), msg["count"].GetUint());
    return this->sendSimpleStatus(true);
}