#ifndef ISNODEFAILED_H
#define ISNODEFAILED_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_IsNodeFailed : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "isnodefailed";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_IsNodeFailed(QObject *parent = nullptr);
};

#endif // ISNODEFAILED_H