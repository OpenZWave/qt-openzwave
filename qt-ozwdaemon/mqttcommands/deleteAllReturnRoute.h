#ifndef DELETEALLRETURNROUTE_H
#define DELETEALLRETURNROUTE_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_DeleteAllReturnRoute : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "deleteallreturnroute";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_DeleteAllReturnRoute(QObject *parent = nullptr);
};

#endif // DELETEALLRETURNROUTE_H