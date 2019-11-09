#ifndef REQUESTNODEDYNAMIC_H
#define REQUESTNODEDYNAMIC_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestNodeDynamic : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "RequestNodeDynamic";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RequestNodeDynamic(QObject *parent = nullptr);
};

#endif // REQUESTNODEDYNAMIC_H