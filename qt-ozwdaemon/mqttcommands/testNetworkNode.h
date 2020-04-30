#ifndef TESTNETWORKNODE_H
#define TESTNETWORKNODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_TestNetworkNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "testnetworknode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_TestNetworkNode(QObject *parent = nullptr);
};

#endif // PING_H