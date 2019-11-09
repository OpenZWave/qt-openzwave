#ifndef CHECKLATESTMFSREVISION_H
#define CHECKLATESTMFSREVISION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_CheckLatestMFSRevision : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "CheckLatestMFSRevision";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_CheckLatestMFSRevision(QObject *parent = nullptr);
};

#endif // CHECKLATESTMFSREVISION_H