#ifndef REPLACEFAILEDNODE_H
#define REPLACEFAILEDNODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_ReplaceFailedNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "ReplaceFailedNode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_ReplaceFailedNode(QObject *parent = nullptr);
};

#endif // REPLACEFAILEDNODE_H