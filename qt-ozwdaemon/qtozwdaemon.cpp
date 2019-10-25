#include <QtDebug>
#include "qtozwdaemon.h"


qtozwdaemon::qtozwdaemon(QObject *parent) : QObject(parent)
{
    this->m_openzwave = new QTOpenZwave(this);
    this->m_qtozwmanager = this->m_openzwave->GetManager();
    QObject::connect(this->m_qtozwmanager, &QTOZWManager::ready, this, &qtozwdaemon::QTOZW_Ready);
    this->m_qtozwmanager->initilizeSource(true);

    this->m_client = new QMqttClient(this);
    this->m_client->setHostname("10.51.107.19");
    this->m_client->setPort(1883);

    connect(this->m_client, &QMqttClient::stateChanged, this, &qtozwdaemon::updateLogStateChange);
    connect(this->m_client, &QMqttClient::disconnected, this, &qtozwdaemon::brokerDisconnected);

    connect(this->m_client, &QMqttClient::messageReceived, this, &qtozwdaemon::handleMessage);
    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
        qDebug() << content;
    });
    this->m_client->connectToHost();
}

void qtozwdaemon::QTOZW_Ready() {
    qDebug() << "Ready";
}
void qtozwdaemon::startOZW() {
    if (getSerialPort().size() == 0) {
        qWarning() << "Serial Port is not Set";
        return;
    }
    //this->m_qtozwmanager->open(this->getSerialPort());
}

void qtozwdaemon::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change: " )
                    + QString::number(m_client->state());
    qDebug() << content;
    if (this->m_client->state() == QMqttClient::ClientState::Connected) {
        this->m_client->subscribe(QMqttTopicFilter("/OpenZWave/commands"));
        this->m_client->publish(QMqttTopicName("/OpenZWave/commands"), QString("testhaha").toLocal8Bit());
    }

}

void qtozwdaemon::brokerDisconnected()
{
    qDebug() << "Disconnnected";
}

void qtozwdaemon::handleMessage(const QByteArray &message, const QMqttTopicName &topic) {
    qDebug() << "Received: " << topic.name() << ":" << message;
}
