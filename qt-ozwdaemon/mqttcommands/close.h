#ifndef CLOSE_H
#define CLOSE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_Close : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "close";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_Close(QObject *parent = nullptr);
};

#endif // CLOSE_H