#ifndef REQUESTALLCONFIGPARAM_H
#define REQUESTALLCONFIGPARAM_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestAllConfigParam : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "requestallconfigparam";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RequestAllConfigParam(QObject *parent = nullptr);
};

#endif // REQUESTALLCONFIGPARAM_H