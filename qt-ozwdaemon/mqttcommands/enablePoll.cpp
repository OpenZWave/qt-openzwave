#include "mqttcommands/enablePoll.h"

MqttCommand_EnablePoll::MqttCommand_EnablePoll(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "ValueIDKey" << "Intensity";
}
MqttCommand* MqttCommand_EnablePoll::Create(QObject *parent) {
    return new MqttCommand_EnablePoll(parent);
}

bool MqttCommand_EnablePoll::processMessage(rapidjson::Document &msg) {
    if (!this->checkValue(msg, "ValueIDKey")) {
        return this->sendSimpleStatus(false, "Invalid ValueIDKey Number");
    }

    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->enablePoll(msg["ValueIdKey"].GetUint64(), msg["Intensity"].GetUint()));
}