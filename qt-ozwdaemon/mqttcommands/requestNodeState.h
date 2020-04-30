#ifndef REQUESTNODESTATE_H
#define REQUESTNODESTATE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestNodeState : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "requestnodestate";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RequestNodeState(QObject *parent = nullptr);
};

#endif // REQUESTNODESTATE_H