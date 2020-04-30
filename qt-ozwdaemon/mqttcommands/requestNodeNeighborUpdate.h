#ifndef REQUESTNODENEIGHBORUPDATE_H
#define REQUESTNODENEIGHBORUPDATE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_RequestNodeNeighborUpdate : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "requestnodeneighborupdate";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_RequestNodeNeighborUpdate(QObject *parent = nullptr);
};

#endif // REQUESTNODENEIGHBORUPDATE_H