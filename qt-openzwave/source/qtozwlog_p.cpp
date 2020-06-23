//-----------------------------------------------------------------------------
//
//	qtozwlog_p.cpp
//
//	OpenZWave Log - Internal Class to manage the Model
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

#include <QObject>
#include "include/qtozwlog_p.h"
#include "include/qtozw_logging.h"


QTOZWLog_Internal::QTOZWLog_Internal(QObject *parent) :
    QTOZWLogSimpleSource(parent),
    m_maxLogLength(10000)
{
    /* As the OZW may call Write Method from the OZW Thread context, we need to send the message using a QueuedConnection so its ends up in the QT Main Loop Thread for sending
     */
    connect(this, &QTOZWLog_Internal::sendLogLine, this, &QTOZWLog_Internal::newLogLine);

    this->setObjectName("QTOZWLog");

    this->m_logData.reserve(1000);

    OpenZWave::Log::SetLoggingClass(static_cast<OpenZWave::i_LogImpl*>(this));

}

QTOZWLog_Internal::~QTOZWLog_Internal() 
{
    
}



void QTOZWLog_Internal::Write(OpenZWave::LogLevel _level, uint8 const _nodeId, char const* _format, va_list _args) {
    char lineBuf[1024] = { 0 };
    if (_format != nullptr && _format[0] != '\0') {
        va_list saveargs;
        va_copy(saveargs, _args);
        vsnprintf(lineBuf, sizeof(lineBuf), _format, _args);
        va_end(saveargs);
    }
    if (strlen(lineBuf) == 0) {
        /* OZW Has lots of Empty Lines to easy visibility, But lets ignore them */
        return;
    }

    LogLevels::Level qtozwlevel = LogLevels::Level::Debug;
    switch (_level) {
        case OpenZWave::LogLevel_Invalid:
        case OpenZWave::LogLevel_None:
        case OpenZWave::LogLevel_StreamDetail:
        case OpenZWave::LogLevel_Internal:
        {
            /*  we don't handle this, return */
            return;
        }
        case OpenZWave::LogLevel_Always: {
            qtozwlevel = LogLevels::Level::Always;
            qCDebug(libopenzwave) << "Always - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Fatal: {
            qtozwlevel = LogLevels::Level::Fatal;
            qCCritical(libopenzwave) << "Fatal - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Error: {
            qtozwlevel = LogLevels::Level::Error;
            qCCritical(libopenzwave) << "Error - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Warning: {
            qtozwlevel = LogLevels::Level::Warning;
            qCWarning(libopenzwave) << "Warning - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Alert: {
            qtozwlevel = LogLevels::Level::Alert;
            qCWarning(libopenzwave) << "Alert - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Info: {
            qtozwlevel = LogLevels::Level::Info;
            qCInfo(libopenzwave) << "Info - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Detail: {
            qtozwlevel = LogLevels::Level::Detail;
            qCDebug(libopenzwave) << "Detail - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Debug: {
            qtozwlevel = LogLevels::Level::Debug;
            qCDebug(libopenzwave) << "Debug - Node:" << _nodeId << lineBuf;
            break;
        }
    }

    QTOZWLog::QTOZW_LogEntry le;
    le.s_msg = lineBuf;
    le.s_node = _nodeId;
    le.s_time = QDateTime::currentDateTime();
    le.s_level = qtozwlevel;
    if (static_cast<quint32>(this->m_logData.size()) >= this->m_maxLogLength) {
        qCDebug(logModel) << "popping Log Mesages";
        this->m_logData.pop_front();
        emit logLinesPopped(1);
    }
    this->m_logData.push_back(le);
    if (this->m_logData.size() == this->m_logData.capacity()-1) {
        qCDebug(logModel) << "Increasing Logging Capacity to " << this->m_logData.capacity()+1000;
        this->m_logData.reserve(this->m_logData.capacity()+1000);
    }
    emit sendLogLine(le.s_time, le.s_level, le.s_node, le.s_msg);
}
void QTOZWLog_Internal::QueueDump() {

}
void QTOZWLog_Internal::QueueClear() {

}
void QTOZWLog_Internal::SetLoggingState(OpenZWave::LogLevel _saveLevel, OpenZWave::LogLevel _queueLevel, OpenZWave::LogLevel _dumpTrigger) {
    /* do Nothing. We filter via the Model Class if needed */
    Q_UNUSED(_saveLevel);
    Q_UNUSED(_queueLevel);
    Q_UNUSED(_dumpTrigger);
}
void QTOZWLog_Internal::SetLogFileName(const std::string &_filename) {
    /* do Nothing. Not used */
    Q_UNUSED(_filename);
}

quint32 QTOZWLog_Internal::getLogCount() 
{
    return this->m_logData.count();
};

bool QTOZWLog_Internal::syncroniseLogs()
{
    qCDebug(logModel) << "QTOZWLog_Internal::syncroniseLogs() called";
    this->m_shadowLogData = this->m_logData;
    this->sendSyncLogsBatch();
    connect(&this->m_syncLogTrigger, &QTimer::timeout, this, &QTOZWLog_Internal::sendSyncLogsBatch);
    emit this->syncStatus(this->m_shadowLogData.size(), false);
    this->m_syncLogTrigger.start(500);
    return false;
};

void QTOZWLog_Internal::sendSyncLogsBatch() 
{
    qCDebug(logModel) << "QTOZWLog_Internal::sendSyncLogsBatch running";
    QTOZWLog::QTOZW_LogEntry le;
    static QVectorIterator<QTOZWLog::QTOZW_LogEntry> i(this->m_shadowLogData);
    static bool started = false;
    int j = 0;
    if (started == false) {
        started = true;
        i.toBack();
    }
    while (i.hasPrevious()) {
        le = i.previous();
        emit this->syncronizedLogLine(le.s_time, le.s_level, le.s_node, le.s_msg);
        j++;
        if (j % 100 == 0) return;
    }
    emit this->syncStatus(this->m_shadowLogData.size(), true);
    this->m_shadowLogData.clear();
    this->m_syncLogTrigger.stop();
    qCDebug(logModel) << "QTOZWLog_Internal::sendSyncLogsBatch() Finished";
}

quint32 QTOZWLog_Internal::getLogBufSize() const
{
    return this->m_maxLogLength;
}

void QTOZWLog_Internal::setLogBufSize(quint32 size) {
    if (this->m_maxLogLength != size)
    {
        if (this->m_maxLogLength > size)
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
