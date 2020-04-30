#ifndef REMOVEASSOCIATION_H
#define REMOVEASSOCIATION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RemoveAssociation : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "removeassociation";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RemoveAssociation(QObject *parent = nullptr);
};

#endif // PING_H