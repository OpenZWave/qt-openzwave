#ifndef CHECKLATESTCONFIGFILEREVISION_H
#define CHECKLATESTCONFIGFILEREVISION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_CheckLatestConfigFileRevision : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "CheckLatestConfigFileRevision";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_CheckLatestConfigFileRevision(QObject *parent = nullptr);
};

#endif // CHECKLATESTCONFIGFILEREVISION_H