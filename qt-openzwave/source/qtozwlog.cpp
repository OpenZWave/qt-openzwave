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
    connect(this->d_ptr_internal, &QTOZWLog_Internal::logBufSizeChanged, this, &QTOZWLog::logBufSizeChanged);

    connectSignals();

    return true;
}


bool QTOZWLog::initilizeReplica(QRemoteObjectNode *m_replicaNode) {
    this->setConnectionType(ConnectionType::Type::Remote);

    initilizeBase();

    this->d_ptr_replica = m_replicaNode->acquire<QTOZWLogReplica>("QTOZWLog");

    connectSignals();
    QObject::connect(this->d_ptr_replica, &QTOZWLogReplica::stateChanged, this, &QTOZWLog::onStateChange);
    QObject::connect(this->d_ptr_replica, &QTOZWLogReplica::newLogLine, this, &QTOZWLog::insertLogLine);
    QObject::connect(this->d_ptr_replica, &QTOZWLogReplica::syncronizedLogLine, this, &QTOZWLog::syncLogMessages);
    return true;
}

void QTOZWLog::connectSignals() {
    CONNECT_DPTR(newLogLine);
    CONNECT_DPTR(syncronizedLogLine);
    CONNECT_DPTR(syncStatus);
}

/* This only runs for Remote Connections. For Local Connections, 
 * the Log Messages are stored in the QTOZWLog_Internal 
 * Instance
 */
void QTOZWLog::insertLogLine(QDateTime time, LogLevels::Level level, quint8 node, QString msg) 
{
    QTOZWLog::QTOZW_LogEntry le;
    le.s_msg = msg;
    le.s_node = node;
    le.s_time = time;
    le.s_level = level;
    if (static_cast<quint32>(this->m_logData.size()) >= this->m_maxLogLength) {
        this->m_logData.pop_front();
        emit logLinesPopped(1);
    }
    this->m_logData.push_back(le);
    if (this->m_logData.size() == this->m_logData.capacity()-1) {
        qCDebug(logModel) << "Increasing Logging Capacity to " << this->m_logData.capacity()+1000;
        this->m_logData.reserve(this->m_logData.capacity()+1000);
    }
}

void QTOZWLog::syncLogMessages(QDateTime time, LogLevels::Level level, quint8 node, QString msg)
{
    QTOZWLog::QTOZW_LogEntry le;
    le.s_msg = msg;
    le.s_node = node;
    le.s_time = time;
    le.s_level = level;
    if (static_cast<quint32>(this->m_logData.size()) >= this->m_maxLogLength) {
        /* if we hit our Limit, these are older messages comming in, so just drop them */
        return;
    }
    this->m_logData.push_front(le);
    if (this->m_logData.size() == this->m_logData.capacity()-1) {
        qCDebug(logModel) << "Increasing Logging Capacity to " << this->m_logData.capacity()+1000;
        this->m_logData.reserve(this->m_logData.capacity()+1000);
    }
}

quint32 QTOZWLog::getLogCount() const
{
    CALL_DPTR_RTN(getLogCount(), quint32, 0);
}

bool QTOZWLog::syncroniseLogs() 
{
    if (!this->isReady()) {
        qCDebug(logModel) << "Logs not Ready";
        return false; 
    }
    qCDebug(logModel) << "SyncronizeLogs Called for " << this->getLogCount() << " Messages";

    if (this->getConnectionType() == ConnectionType::Type::Local) 
    {
        /* if our connection is Local, Nothing to do */
        qCDebug(logModel) << "Logs are Local - Nothing to do";
        return true;
    } 
    else 
    { 
        /* if its a Remote Connection, we have to reset our Local Copy and ask the Source to send us
         * the Log Entries 
         */
        qCDebug(logModel) << "Logs are Remote - Asking for Logs";

        this->m_logData.clear();
        emit this->logCleared();
        QRemoteObjectPendingReply<bool> res = this->d_ptr_replica->syncroniseLogs(); 
        res.waitForFinished(3000); 
        return res.returnValue(); 
    }
}

const QVector<QTOZWLog::QTOZW_LogEntry> &QTOZWLog::getLogEntries() {
    if (!this->isReady()) {
        return this->m_logData;
    }
    if (this->getConnectionType() == ConnectionType::Type::Local)
    {
        return this->d_ptr_internal->m_logData;
    }
    else
    {
        return this->m_logData;
    }
}

quint32 QTOZWLog::getLogBufSize() const 
{
    if (!this->isReady())
        return 0;
    if (this->getConnectionType() == ConnectionType::Type::Local)
    {
        return this->d_ptr_internal->getLogBufSize();
    }
    else
    {
        return this->m_maxLogLength;
    }
}

void QTOZWLog::setLogBufSize(quint32 size)
{
    if (this->getConnectionType() == ConnectionType::Type::Local)
    {
        this->d_ptr_internal->setLogBufSize(size);
    }
    else
    {
        if (this->m_maxLogLength != size) {
            /* if its smaller, then we need to pop some items of the existing list */
            if (this->m_maxLogLength > size )
            {
                quint32 numbertopop = this->m_maxLogLength - size;
                if (numbertopop > static_cast<quint32>(this->m_logData.size()))
                    numbertopop = this->m_logData.size();

                qCDebug(logModel) << "Removing " << numbertopop << "entries from log";
                this->m_logData.remove(0, numbertopop);
                emit this->logLinesPopped(numbertopop);
            }
            this->m_maxLogLength = size;
            emit this->logBufSizeChanged(size);
        }
    }
}




QTOZWLogModel::QTOZWLogModel(QTOZWLog *qtozwlog, QObject *parent) :
    QAbstractTableModel(parent),
    m_qtozwlog(qtozwlog)
{
    if (this->m_qtozwlog->getLogEntries().count() > 0) {
        this->beginInsertRows(QModelIndex(), 0, this->m_qtozwlog->getLogEntries().count());
        this->endInsertRows();
    }
    connect(this->m_qtozwlog, &QTOZWLog::newLogLine, this, &QTOZWLogModel::insertLogMessage);
    connect(this->m_qtozwlog, &QTOZWLog::syncronizedLogLine, this, &QTOZWLogModel::syncLogMessage);
    connect(this->m_qtozwlog, &QTOZWLog::logCleared, this, &QTOZWLogModel::resetModel);
    connect(this->m_qtozwlog, &QTOZWLog::logLinesPopped, this, &QTOZWLogModel::logsPoppped);
}

int QTOZWLogModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return this->m_qtozwlog->getLogEntries().count();
}

int QTOZWLogModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return LogColumns::Count;
}

QVariant QTOZWLogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QTOZWLog::QTOZW_LogEntry le = this->getLogData(index.row()-1);
        if (!le.s_time.isValid()) {
            qCWarning(logModel) << "QTOZWLog::data asked for a invalid row " << index.row() << " size: " << rowCount(QModelIndex());
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

QVariant QTOZWLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

Qt::ItemFlags QTOZWLogModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractTableModel::flags(index);
}

const QTOZWLog::QTOZW_LogEntry QTOZWLogModel::getLogData(const int pos) const {
    if (this->m_qtozwlog->getLogEntries().count() >= pos)
        return this->m_qtozwlog->getLogEntries().at(pos+1);
    qCWarning(logModel) << "Can't find LogEntry at " << pos << " size:" << this->m_qtozwlog->getLogEntries().count();
    return QTOZWLog::QTOZW_LogEntry();
}

bool QTOZWLogModel::insertLogMessage(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg) 
{
    Q_UNUSED(time);
    Q_UNUSED(level);
    Q_UNUSED(s_node);
    Q_UNUSED(s_msg);
    this->beginInsertRows(QModelIndex(), this->m_qtozwlog->getLogEntries().count(), this->m_qtozwlog->getLogEntries().count());
    this->endInsertRows();
    return true;
}

bool QTOZWLogModel::syncLogMessage(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg) 
{
    Q_UNUSED(time);
    Q_UNUSED(level);
    Q_UNUSED(s_node);
    Q_UNUSED(s_msg);
    this->beginInsertRows(QModelIndex(), 0, 0);
    this->endInsertRows();
    return true;
}


void QTOZWLogModel::resetModel() {
    this->beginResetModel();
    this->endResetModel();
    
}

void QTOZWLogModel::logsPoppped(quint32 size)
{
    this->beginRemoveRows(QModelIndex(), 0, size);
    this->endRemoveRows();
}
