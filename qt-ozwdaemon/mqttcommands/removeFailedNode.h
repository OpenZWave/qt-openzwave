#ifndef REMOVEFAILEDNODE_H
#define REMOVEFAILEDNODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RemoveFailedNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "RemoveFailedNode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_RemoveFailedNode(QObject *parent = nullptr);
};

#endif // PING_H