#include "mqttcommands/refreshValue.h"

MqttCommand_RefreshValue::MqttCommand_RefreshValue(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "ValueIDKey";
}
MqttCommand* MqttCommand_RefreshValue::Create(QObject *parent) {
    return new MqttCommand_RefreshValue(parent);
}

bool MqttCommand_RefreshValue::processMessage(rapidjson::Document &msg) {
    if (!this->checkValue(msg, "ValueIDKey")) {
        return this->sendSimpleStatus(false, "Invalid ValueIDKey Number");
    }

    QTOZWManager *mgr = getOZWManager();
    return this->sendSimpleStatus(mgr->refreshValue(msg["ValueIDKey"].GetUint64()));
}