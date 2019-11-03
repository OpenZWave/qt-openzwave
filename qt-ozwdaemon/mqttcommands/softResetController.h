#ifndef SOFTRESETCONTROLLER_H
#define SOFTRESETCONTROLLER_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_SoftResetController : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "SoftResetController";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_SoftResetController(QObject *parent = nullptr);
};

#endif // SOFTRESETCONTROLLER_H