#ifndef REMOVENODE_H
#define REMOVENODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RemoveNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "removenode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RemoveNode(QObject *parent = nullptr);
};

#endif // PING_H