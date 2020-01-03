#ifndef HEALNETWORKNODE_H
#define HEALNETWORKNODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_HealNetworkNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "HealNetworkNode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_HealNetworkNode(QObject *parent = nullptr);
};

#endif // HEALNETWORKNODE_H