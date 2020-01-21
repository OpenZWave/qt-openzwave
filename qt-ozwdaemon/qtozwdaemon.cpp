#include <QtDebug>
#include "qtozwdaemon.h"


qtozwdaemon::qtozwdaemon(QObject *parent) : QObject(parent)
{
    QRegularExpression re("^(0[xX][a-fA-F0-9]{2}[ ,]*){16}$");
    
    QString NetworkKeyTest = qgetenv("OZW_NETWORK_KEY");
    QString NetworkKey;
    QRegularExpressionMatch match = re.match(NetworkKeyTest);
    if (match.hasMatch()) {
        NetworkKey = NetworkKeyTest;
        qInfo() << "Network Key Specified in Enviroment is Valid";
    } else {
        qWarning() << "Network Key Specified in Enviroment is Invalid";
    }
    
    QFile nwk_file("/run/secrets/OZW_Network_Key");
    if (nwk_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        NetworkKeyTest = nwk_file.readLine().trimmed();
        nwk_file.close();
        match = re.match(NetworkKeyTest);
        if (match.hasMatch()) {
            NetworkKey = NetworkKeyTest;
            qInfo() << "Network Key From File is Valid - Using File";
        } else {
            if (NetworkKey.isEmpty()) {
                qWarning() << "Network Key From File in Invalid - No Valid Network Key Found in Enviroment or File";
            }
        }
    } else {
        qInfo() << "Didn't Find Network Key File. Skipping";
    }
    if (!NetworkKey.isEmpty()) {
        qInfo() << "We Have what appears to be a valid Network Key - Passing to OZW";
    }

    this->m_openzwave = new QTOpenZwave(this);
    this->m_qtozwmanager = this->m_openzwave->GetManager();
    QObject::connect(this->m_qtozwmanager, &QTOZWManager::ready, this, &qtozwdaemon::QTOZW_Ready);
    this->m_qtozwmanager->initilizeSource(true);
    if (!NetworkKey.isEmpty()) {
        this->m_qtozwmanager->getOptions()->setNetworkKey(NetworkKey);
    }
}

void qtozwdaemon::QTOZW_Ready() {
    qDebug() << "Ready";
}
void qtozwdaemon::startOZW() {
    if (getSerialPort().size() == 0) {
        qWarning() << "Serial Port is not Set";
        return;
    }
    this->m_qtozwmanager->open(this->getSerialPort());
}

QTOZWManager *qtozwdaemon::getManager() {
    return this->m_qtozwmanager;
}

QTOpenZwave *qtozwdaemon::getQTOpenZWave() {
    return this->m_openzwave;
}
