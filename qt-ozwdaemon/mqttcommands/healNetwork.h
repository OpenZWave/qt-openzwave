#ifndef HEALNETWORK_H
#define HEALNETWORK_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_HealNetwork : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "HealNetwork";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_HealNetwork(QObject *parent = nullptr);
};

#endif // PING_H