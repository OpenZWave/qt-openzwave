#ifndef ADDNODESECURE_H
#define ADDNODESECURE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_AddNodeSecure : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "AddNodeSecure";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_AddNodeSecure(QObject *parent = nullptr);
};

#endif // PING_H