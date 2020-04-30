#ifndef SYNCNODENEIGHBORS_H
#define SYNCNODENEIGHBORS_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_SyncroniseNodeNeighbors : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "syncronisenodeneighbors";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_SyncroniseNodeNeighbors(QObject *parent = nullptr);
};

#endif // SYNCNODENEIGHBORS_H