#ifndef DISABLEPOLL_H
#define DISABLEPOLL_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_DisablePoll : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "disablePoll";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_DisablePoll(QObject *parent = nullptr);
};

#endif // PING_H