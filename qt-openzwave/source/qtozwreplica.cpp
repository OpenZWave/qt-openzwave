#include "qt-openzwave/qtozwreplica.h"
#include "qtozw_logging.h"


QTOZWReplicaBase::QTOZWReplicaBase(ConnectionType::Type connectionType, QObject *parent) :
    QObject(parent),
    m_connectionType(connectionType),
    m_ready(false)
{

}
QTOZWReplicaBase::~QTOZWReplicaBase() 
{

}

ConnectionType::Type QTOZWReplicaBase::getConnectionType() const
{ 
    return this->m_connectionType; 
}

void QTOZWReplicaBase::setConnectionType(ConnectionType::Type connectionType)
{
    if (this->m_connectionType != connectionType) {
        this->m_connectionType = connectionType;
        emit connectionTypeChanged(this->m_connectionType);
    }
}

bool QTOZWReplicaBase::isReady() const
{ 
    return this->m_ready; 
}

void QTOZWReplicaBase::setReady(bool ready) 
{ 
    if (this->m_ready != ready) { 
        qCDebug(manager) << this->metaObject()->className() << "Ready!";
        this->m_ready = ready; 
        emit readyChanged(this->m_ready);
    }
}     
