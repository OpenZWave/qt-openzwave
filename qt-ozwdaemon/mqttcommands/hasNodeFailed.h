#ifndef HASNODEFAILED_H
#define HASNODEFAILED_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_HasNodeFailed : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "HadNodeFailed";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_HasNodeFailed(QObject *parent = nullptr);
};

#endif // HASNODEFAILED_H