#include "mqttcommands/getPollInterval.h"

MqttCommand_GetPollInterval::MqttCommand_GetPollInterval(QObject *parent) :
    MqttCommand(parent)
{
}
MqttCommand* MqttCommand_GetPollInterval::Create(QObject *parent) {
    return new MqttCommand_GetPollInterval(parent);
}

bool MqttCommand_GetPollInterval::processMessage(rapidjson::Document &msg) {
    Q_UNUSED(msg);
    QTOZWManager *mgr = getOZWManager();
    rapidjson::Document js;
    QT2JS::SetString(js, "status", "ok");
    QT2JS::SetInt64(js, "pollInterval", mgr->getPollInterval());
    emit sendCommandUpdate(GetCommand(), js);
    return true;
}