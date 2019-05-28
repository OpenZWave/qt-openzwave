#include <QtDebug>
#include "qtozwdaemon.h"

qtozwdaemon::qtozwdaemon(QObject *parent) : QObject(parent)
{
    this->m_openzwave = new QTOpenZwave(this);
    this->m_qtozwmanager = this->m_openzwave->GetManager();
    QObject::connect(this->m_qtozwmanager, &QTOZWManager::ready, this, &qtozwdaemon::QTOZW_Ready);
    this->m_qtozwmanager->initilizeSource(true);
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
