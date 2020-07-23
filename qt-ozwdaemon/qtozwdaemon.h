#ifndef QTOZWDAEMON_H
#define QTOZWDAEMON_H

#include <QObject>
#include <QString>
#include <QSocketNotifier>


#include <qt-openzwave/qtopenzwave.h>
#include <qt-openzwave/qtozwmanager.h>

class qtozwdaemon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serialPort READ getSerialPort WRITE setSerialPort);
    Q_PROPERTY(QString configPath READ getConfigPath WRITE setConfigPath);
    Q_PROPERTY(QString userPath READ getUserPath WRITE setUserPath);
public:
    enum ExitCodes {
        EXIT_NORMAL = 0,
        EXIT_NOMQTTPASS = -1,
        EXIT_ANOTHERINSTANCE = -2,
        EXIT_MQTTDISCONNECTED = -3,
        EXIT_OZWFAILED = -4,
        EXIT_SETUPFAILED = -5,
        EXIT_CRASH = -6
    };
    Q_ENUM(ExitCodes);

    explicit qtozwdaemon(QString configPath = QString(), QString userPath = QString(), QObject *parent = nullptr);
    QString getSerialPort() { return this->m_serialPort; }
    void setSerialPort(QString serialPort) { this->m_serialPort = serialPort;}
    void startOZW();
    QTOZWManager *getManager();
    QTOpenZwave *getQTOpenZWave();
    static void termSignalHandler(int unused);
    QString getConfigPath() { return this->m_configPath; }
    QString getUserPath() { return this->m_userPath; }
signals:

public slots:
    void QTOZW_Ready(bool ready);
    void handleSigTerm();
    void aboutToQuit();
    void setConfigPath(QString configPath) { this->m_configPath = configPath; }
    void setUserPath(QString userPath) { this->m_userPath = userPath; }
    void nodeQueriesComplete(quint8 node);


private:
    QTOpenZwave *m_openzwave;
    QTOZWManager *m_qtozwmanager;
    QString m_serialPort;
    static int sigtermFd[2];
    QSocketNotifier *snTerm;
    QString m_configPath;
    QString m_userPath;
};

#endif // QTOZWDAEMON_H
