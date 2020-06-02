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
    Q_PROPERTY(QString serialPort READ getSerialPort WRITE setSerialPort)
public:
    explicit qtozwdaemon(QObject *parent = nullptr);
    QString getSerialPort() { return this->m_serialPort; }
    void setSerialPort(QString serialPort) { this->m_serialPort = serialPort;}
    void startOZW();
    QTOZWManager *getManager();
    QTOpenZwave *getQTOpenZWave();
    static void termSignalHandler(int unused);
signals:

public slots:
    void QTOZW_Ready(bool ready);
    void handleSigTerm();
    void aboutToQuit();

private:
    QTOpenZwave *m_openzwave;
    QTOZWManager *m_qtozwmanager;
    QString m_serialPort;
    static int sigtermFd[2];
    QSocketNotifier *snTerm;
};

#endif // QTOZWDAEMON_H
