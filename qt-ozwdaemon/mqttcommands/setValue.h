#ifndef SETVALUE_H
#define SETVALUE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_SetValue : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "SetValue";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_SetValue(QObject *parent = nullptr);
};

#endif // SETVALUE_H