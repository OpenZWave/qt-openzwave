#ifndef HARDRESETCONTROLLER_H
#define HARDRESETCONTROLLER_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_HardResetController : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "hardresetcontroller";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_HardResetController(QObject *parent = nullptr);
};

#endif // HARDRESETCONTROLLER_H