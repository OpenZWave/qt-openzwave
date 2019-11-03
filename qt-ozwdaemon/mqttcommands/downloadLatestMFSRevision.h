#ifndef DOWNLOADLATESTMFSREVISION_H
#define DOWNLOADLATESTMFSREVISION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_DownloadLatestMFSRevision : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "DownloadLatestMFSRevision";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_DownloadLatestMFSRevision(QObject *parent = nullptr);
};

#endif // DOWNLOADLATESTMFSREVISION_H