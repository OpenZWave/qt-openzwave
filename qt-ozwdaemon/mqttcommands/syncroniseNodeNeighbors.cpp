#include "mqttcommands/syncroniseNodeNeighbors.h"

MqttCommand_SyncroniseNodeNeighbors::MqttCommand_SyncroniseNodeNeighbors(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "node";
}
MqttCommand* MqttCommand_SyncroniseNodeNeighbors::Create(QObject *parent) {
    return new MqttCommand_SyncroniseNodeNeighbors(parent);
}

bool MqttCommand_SyncroniseNodeNeighbors::processMessage(rapidjson::Document &msg) {
    if (!this->checkNode(msg, "node")) {
        return this->sendSimpleStatus(false, "Invalid Node Number");
    }

    QTOZWManager *mgr = getOZWManager();
    mgr->syncroniseNodeNeighbors(msg["node"].GetInt());
    return this->sendSimpleStatus(true);
}