#ifndef REQUESTNODENEIGHBORUPDATE_H
#define REQUESTNODENEIGHBORUPDATE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestNodeNeighborUpdate : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "RequestNodeNeighborUpdate";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_RequestNodeNeighborUpdate(QObject *parent = nullptr);
};

#endif // REQUESTNODENEIGHBORUPDATE_H