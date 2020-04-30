#ifndef GETPOLLINTERVAL_H
#define GETPOLLINTERVAL_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_GetPollInterval : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "getpollinterval";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_GetPollInterval(QObject *parent = nullptr);
};

#endif // GETPOLLINTERVAL_H