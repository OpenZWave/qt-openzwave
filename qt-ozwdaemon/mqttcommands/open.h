#ifndef OPEN_H
#define OPEN_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_Open : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "open";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_Open(QObject *parent = nullptr);
};

#endif // PING_H