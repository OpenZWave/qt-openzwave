#include "mqttcommands/disablePoll.h"

MqttCommand_DisablePoll::MqttCommand_DisablePoll(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "ValueIDKey";
}
MqttCommand* MqttCommand_DisablePoll::Create(QObject *parent) {
    return new MqttCommand_DisablePoll(parent);
}

bool MqttCommand_DisablePoll::processMessage(rapidjson::Document &msg) {
    if (!this->checkValue(msg, "ValueIDKey")) {
        return this->sendSimpleStatus(false, "Invalid ValueIDKey Number");
    }
    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->disablePoll(msg["ValueIdKey"].GetUint64()));
}