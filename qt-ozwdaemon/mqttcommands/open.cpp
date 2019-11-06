#include "mqttcommands/open.h"

MqttCommand_Open::MqttCommand_Open(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredStringFields << "serialport";
}
MqttCommand* MqttCommand_Open::Create(QObject *parent) {
    return new MqttCommand_Open(parent);
}

bool MqttCommand_Open::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->open(msg["serialport"].toString())) {
        QJsonObject js;
        js["status"] = "ok";
        emit sendCommandUpdate(GetCommand(), js);
        return true;
    }
    QJsonObject js;
    js["status"] = "failed";
    emit sendCommandUpdate(GetCommand(), js);
    return false;
}