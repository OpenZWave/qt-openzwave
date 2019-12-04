#include "mqttcommands/setPollInterval.h"

MqttCommand_SetPollInterval::MqttCommand_SetPollInterval(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "interval";
    this->m_requiredBoolFields << "intervalBetweenPoll";
}
MqttCommand* MqttCommand_SetPollInterval::Create(QObject *parent) {
    return new MqttCommand_SetPollInterval(parent);
}

bool MqttCommand_SetPollInterval::processMessage(rapidjson::Document &msg) {
    QTOZWManager *mgr = getOZWManager();
    mgr->setPollInterval(msg["interval"].GetUint(), msg["intervalBetweenPoll"].GetBool());
    return this->sendSimpleStatus(true);
}