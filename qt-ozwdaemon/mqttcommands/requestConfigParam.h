#ifndef REQUESTCONFIGPARAM_H
#define REQUESTCONFIGPARAM_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestConfigParam : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "RequestConfigParam";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RequestConfigParam(QObject *parent = nullptr);
};

#endif // PING_H