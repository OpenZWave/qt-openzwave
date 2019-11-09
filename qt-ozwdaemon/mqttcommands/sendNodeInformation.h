#ifndef SENTNODEINFORMATION_H
#define SENTNODEINFORMATION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_SendNodeInformation : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "SendNodeInformation";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_SendNodeInformation(QObject *parent = nullptr);
};

#endif // PING_H