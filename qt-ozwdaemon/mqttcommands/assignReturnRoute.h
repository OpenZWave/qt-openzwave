#ifndef ASSIGNRETURNROUTE_H
#define ASSIGNRETURNROUTE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_AssignReturnRoute : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "assignreturnroute";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_AssignReturnRoute(QObject *parent = nullptr);
};

#endif // PING_H