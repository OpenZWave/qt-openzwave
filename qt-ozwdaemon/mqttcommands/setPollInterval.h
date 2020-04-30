#ifndef SETPOLLINTERVAL_H
#define SETPOLLINTERVAL_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_SetPollInterval : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "setpollinterval";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_SetPollInterval(QObject *parent = nullptr);
};

#endif // SETPOLLINTERVAL_H