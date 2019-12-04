#ifndef REFRESHVALUE_H
#define REFRESHVALUE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RefreshValue : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "refreshValue";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RefreshValue(QObject *parent = nullptr);
};

#endif // REFRESHVALUE_H