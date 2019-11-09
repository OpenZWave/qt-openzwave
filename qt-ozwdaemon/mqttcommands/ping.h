#ifndef PING_H
#define PING_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_Ping : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "Ping";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_Ping(QObject *parent = nullptr);
};

#endif // PING_H
