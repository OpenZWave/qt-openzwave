#include "mqttcommands/enablePoll.h"

MqttCommand_EnablePoll::MqttCommand_EnablePoll(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "ValueIDKey";
}
MqttCommand* MqttCommand_EnablePoll::Create(QObject *parent) {
    return new MqttCommand_SyncroniseNodeNeighbors(parent);
}

bool MqttCommand_EnablePoll::processMessage(rapidjson::Document &msg) {
    if (!this->checkValue(msg, "ValueIDKey")) {
        return this->sendSimpleStatus(false, "Invalid ValueIDKey Number");
    }

    QTOZWManager *mgr = getOZWManager();
    //mgr->syncroniseNodeNeighbors(msg["node"].GetInt());
    return this->sendSimpleStatus(true);
}