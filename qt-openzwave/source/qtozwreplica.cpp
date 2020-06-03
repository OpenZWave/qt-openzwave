#include "qt-openzwave/qtozwreplica.h"


QTOZWReplicaBase::QTOZWReplicaBase(ConnectionType::Type connectionType, QObject *parent) :
    QObject(parent),
    m_connectionType(connectionType)
{

}
QTOZWReplicaBase::~QTOZWReplicaBase() 
{

}

ConnectionType::Type QTOZWReplicaBase::getConnectionType()
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

bool QTOZWReplicaBase::isReady() 
{ 
    return this->m_ready; 
}

void QTOZWReplicaBase::setReady(bool ready) 
{ 
    if (this->m_ready != ready) { 
        this->m_ready = ready; 
        emit readyChanged(this->m_ready);
    }
}     
