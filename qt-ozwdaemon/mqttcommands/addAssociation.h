#ifndef ADDASSOCIATION_H
#define ADDASSOCIATION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_AddAssociation : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "AddAssociation";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_AddAssociation(QObject *parent = nullptr);
};

#endif // PING_H