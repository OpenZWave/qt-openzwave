#include "mqttcommands/sendNodeInformation.h"

MqttCommand_SendNodeInformation::MqttCommand_SendNodeInformation(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredFields << "node";
}
MqttCommand* MqttCommand_SendNodeInformation::Create(QObject *parent) {
    return new MqttCommand_SendNodeInformation(parent);
}

bool MqttCommand_SendNodeInformation::processMessage(QJsonDocument msg) {
    QTOZWManager *mgr = getOZWManager();
    if (mgr->sendNodeInformation(msg["node"].toInt())) {
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