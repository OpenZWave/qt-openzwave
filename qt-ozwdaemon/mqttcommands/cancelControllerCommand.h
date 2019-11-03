#ifndef CANCELCONTROLLERCOMMAND_H
#define CANCELCONTROLLERCOMMAND_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_CancelControllerCommand : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "CancelControllerCommand";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_CancelControllerCommand(QObject *parent = nullptr);
};

#endif // CANCELCONTROLLERCOMMAND_H