//-----------------------------------------------------------------------------
//
//	qtozwlog.cpp
//
//	OpenZWave Log Exposed as a AbstractTableModel
//
//	Copyright (c) 2019 Justin Hammond <Justin@dynam.ac>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of QT-OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#include "qt-openzwave/qtozwlog.h"
#include "qtozwlog_p.h"
#include "qt-openzwave/rep_qtozwlog_replica.h"
#include "qt-openzwave/rep_qtozwlog_source.h"
#include "qtozw_logging.h"

#define REP_INTERNAL_CLASS QTOZWLog_Internal
#define REP_REPLICA_CLASS QTOZWLogReplica
#define REP_PUBLIC_CLASS QTOZWLog

QTOZWLog::QTOZWLog(ConnectionType::Type connectionType, QObject *parent) :
    QTOZWReplicaBase(connectionType, parent),
    m_maxLogLength(100000)
{


}

QTOZWLog::~QTOZWLog() 
{

}

bool QTOZWLog::initilizeBase() {
    return true;
}

bool QTOZWLog::initilizeSource(QRemoteObjectHost *m_sourceNode) {
    this->setConnectionType(ConnectionType::Type::Local);
    initilizeBase();
    this->d_ptr_internal = new QTOZWLog_Internal(this);
    if (m_sourceNode) {
        m_sourceNode->enableRemoting<QTOZWLogSourceAPI>(this->d_ptr_internal);
    }
    connectSignals();
    return true;
}


bool QTOZWLog::initilizeReplica(QRemoteObjectNode *m_replicaNode) {
    this->setConnectionType(ConnectionType::Type::Remote);
    initilizeBase();
    this->d_ptr_replica = m_replicaNode->acquire<QTOZWLogReplica>("QTOZWLog");
    QObject::connect(this->d_ptr_replica, &QTOZWLogReplica::stateChanged, this, &QTOZWLog::onStateChange);
    connectSignals();
    return true;
}

void QTOZWLog::connectSignals() {
    CONNECT_DPTR(newLogLine);
    CONNECT_DPTR(syncronizedLogLine);
}


quint32 QTOZWLog::getLogCount() 
{
    CALL_DPTR_RTN(getLogCount(), quint32, 0);
}

bool QTOZWLog::syncroniseLogs(quint32 records) 
{
    CALL_DPTR_RTN(syncroniseLogs(records), bool, false)
}

#if 0




QTOZW_Log::QTOZW_Log(QObject *parent)
    : QAbstractTableModel(parent),
      m_maxLogLength(100000)
{
}

int QTOZW_Log::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return this->m_logData.count();
}
int QTOZW_Log::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return LogColumns::Count;
}
QVariant QTOZW_Log::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QTOZW_LogEntry le = this->getLogData(index.row()-1);
        if (!le.s_time.isValid()) {
            qCWarning(logModel) << "QTOZW_Log::data asked for a invalid row " << index.row() << " size: " << rowCount(QModelIndex());
            return QVariant();
        }
        switch(static_cast<LogColumns>(index.column())) {
            case TimeStamp: {
                return le.s_time;
            }
            case Node: {
                return le.s_node;
            }
            case Level: {
                return QVariant::fromValue(le.s_level).toString();
            }
            case Message: {
                return le.s_msg;
            }
            case Count: {
                return QVariant();
            }
        }
    }
    return QVariant();

}
QVariant QTOZW_Log::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<LogColumns>(section)) {
            case TimeStamp:
                return tr("TimeStamp");

            case Node:
                return tr("Node");

            case Level:
                return tr("Level");

            case Message:
                return tr("Message");

            case Count:
                return QVariant();
        }
    }
    return QVariant();
}
Qt::ItemFlags QTOZW_Log::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractTableModel::flags(index);
}

QTOZW_Log::QTOZW_LogEntry QTOZW_Log::getLogData(int pos) const {
    if (this->m_logData.size() >= pos)
        return this->m_logData.at(pos+1);
    qCWarning(logModel) << "Can't find LogEntry at " << pos << " size:" << this->m_logData.size();
    return QTOZW_LogEntry();
}

#endif