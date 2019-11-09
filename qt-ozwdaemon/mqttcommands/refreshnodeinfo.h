#ifndef REFRESHNODEINFO_H
#define REFRESHNODEINFO_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RefreshNodeInfo : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "RefreshNodeInfo";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RefreshNodeInfo(QObject *parent = nullptr);
};

#endif // PING_H