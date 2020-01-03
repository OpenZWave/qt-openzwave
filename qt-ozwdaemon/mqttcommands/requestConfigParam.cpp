#include "mqttcommands/requestConfigParam.h"

MqttCommand_RequestConfigParam::MqttCommand_RequestConfigParam(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node" << "param";
}
MqttCommand* MqttCommand_RequestConfigParam::Create(QObject *parent) {
    return new MqttCommand_RequestConfigParam(parent);
}

bool MqttCommand_RequestConfigParam::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }
    QTOZWManager *mgr = getOZWManager();
    mgr->requestConfigParam(msg["node"].GetUint(), msg["param"].GetUint());
    return this->sendSimpleStatus(true);
}