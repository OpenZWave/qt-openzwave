#ifndef PING_H
#define PING_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_Ping : public MqttCommand {
    Q_OBJECT
public:
    MqttCommand_Ping(QObject *parent = nullptr);
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "Ping";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
};

#endif // PING_H
