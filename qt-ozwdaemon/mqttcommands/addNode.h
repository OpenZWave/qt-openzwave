#ifndef ADDNODE_H
#define ADDNODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_AddNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "AddNode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_AddNode(QObject *parent = nullptr);
};

#endif // PING_H