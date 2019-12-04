#ifndef ENABLEPOLL_H
#define ENABLEPOLL_H

#include "mqttcommands/mqttcommands.h"

class MqttCommand_EnablePoll : public MqttCommand {
    Q_OBJECT
public:
    static MqttCommand *Create(QObject *parent = nullptr);
    static QString StaticGetCommand() { return "enablePoll";};
    QString GetCommand() override { return StaticGetCommand(); };
    bool processMessage(rapidjson::Document &) override;
private: 
    MqttCommand_EnablePoll(QObject *parent = nullptr);
};

#endif // ENABLEPOLL_H