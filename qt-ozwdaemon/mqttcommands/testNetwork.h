#ifndef TESTNETWORK_H
#define TESTNETWORK_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_TestNetwork : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "TestNetwork";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_TestNetwork(QObject *parent = nullptr);
};

#endif // TESTNETWORK_H