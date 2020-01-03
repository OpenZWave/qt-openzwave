#ifndef HASNODEFAILED_H
#define HASNODEFAILED_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_HasNodeFailed : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "HasNodeFailed";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_HasNodeFailed(QObject *parent = nullptr);
};

#endif // HASNODEFAILED_H