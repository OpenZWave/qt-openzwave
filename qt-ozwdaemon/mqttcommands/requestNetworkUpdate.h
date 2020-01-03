#ifndef REQUESTNETWORKUPDATE_H
#define REQUESTNETWORKUPDATE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestNetworkUpdate : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "RequestNetworkUpdate";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RequestNetworkUpdate(QObject *parent = nullptr);
};

#endif // REQUESTNETWORKUPDATE_H