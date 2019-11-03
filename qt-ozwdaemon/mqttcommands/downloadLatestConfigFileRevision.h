#ifndef DOWNLOADLATESTCONFIGFILEREVISION_H
#define DOWNLOADLATESTCONFIGFILEREVISION_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_DownloadLatestConfigFileRevision : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "DownloadLatestConfigFileRevision";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(QJsonDocument) override;
private: 
    MqttCommand_DownloadLatestConfigFileRevision(QObject *parent = nullptr);
};

#endif // DOWNLOADLATESTCONFIGFILEREVISION_H