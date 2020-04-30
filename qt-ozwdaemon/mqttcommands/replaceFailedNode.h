#ifndef REPLACEFAILEDNODE_H
#define REPLACEFAILEDNODE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_ReplaceFailedNode : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "replacefailednode";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_ReplaceFailedNode(QObject *parent = nullptr);
};

#endif // REPLACEFAILEDNODE_H