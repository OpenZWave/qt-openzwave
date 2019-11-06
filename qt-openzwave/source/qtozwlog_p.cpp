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

#include "include/qtozwlog_p.h"
#include "include/qtozw_logging.h"

QTOZW_Log_Internal::QTOZW_Log_Internal(QObject *parent) :
    QTOZW_Log(parent)
{
    QObject::connect(this, &QTOZW_Log_Internal::doInsertLogLine, this, &QTOZW_Log_Internal::insertLogLine, Qt::DirectConnection);
    this->m_logData.reserve(1000);
}

bool QTOZW_Log_Internal::insertLogLine(quint8 node, QDateTime timestamp, QTOZW_Log::LogLevels level, QString msg) {
    QTOZW_Log::QTOZW_LogEntry le;
    le.s_msg = msg;
    le.s_node = node;
    le.s_time = timestamp;
    le.s_level = level;
    if (static_cast<quint32>(this->m_logData.size()) >= this->m_maxLogLength) {
        this->beginRemoveRows(QModelIndex(), 0, 0);
        this->m_logData.pop_front();
        this->endRemoveRows();
    }
    this->beginInsertRows(QModelIndex(), this->m_logData.size(), this->m_logData.size());
    this->m_logData.push_back(le);
    this->endInsertRows();
    if (this->m_logData.size() == this->m_logData.capacity()-1) {
        qDebug() << "Increasing Logging Capacity to " << this->m_logData.capacity()+1000;
        this->m_logData.reserve(this->m_logData.capacity()+1000);
    }
    return true;
}


void QTOZW_Log_Internal::Write(OpenZWave::LogLevel _level, uint8 const _nodeId, char const* _format, va_list _args) {
    char lineBuf[1024] = { 0 };
    if (_format != nullptr && _format[0] != '\0') {
        va_list saveargs;
        va_copy(saveargs, _args);
        vsnprintf(lineBuf, sizeof(lineBuf), _format, _args);
        va_end(saveargs);
    }
    QTOZW_Log::LogLevels qtozwlevel = LogLevels::Debug;
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
            qtozwlevel = LogLevels::Always;
            qCDebug(libopenzwave) << "Always - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Fatal: {
            qtozwlevel = LogLevels::Fatal;
            qCCritical(libopenzwave) << "Fatal - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Error: {
            qtozwlevel = LogLevels::Error;
            qCCritical(libopenzwave) << "Error - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Warning: {
            qtozwlevel = LogLevels::Warning;
            qCWarning(libopenzwave) << "Warning - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Alert: {
            qtozwlevel = LogLevels::Alert;
            qCWarning(libopenzwave) << "Alert - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Info: {
            qtozwlevel = LogLevels::Info;
            qCInfo(libopenzwave) << "Info - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Detail: {
            qtozwlevel = LogLevels::Detail;
            qCDebug(libopenzwave) << "Detail - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Debug: {
            qtozwlevel = LogLevels::Debug;
            qCDebug(libopenzwave) << "Debug - Node:" << _nodeId << lineBuf;
            break;
        }
    }
    emit this->doInsertLogLine(_nodeId, QDateTime::currentDateTime(), qtozwlevel, QString(lineBuf));
}
void QTOZW_Log_Internal::QueueDump() {

}
void QTOZW_Log_Internal::QueueClear() {

}
void QTOZW_Log_Internal::SetLoggingState(OpenZWave::LogLevel _saveLevel, OpenZWave::LogLevel _queueLevel, OpenZWave::LogLevel _dumpTrigger) {
    /* do Nothing. We filter via the Model Class if needed */
    Q_UNUSED(_saveLevel);
    Q_UNUSED(_queueLevel);
    Q_UNUSED(_dumpTrigger);
}
void QTOZW_Log_Internal::SetLogFileName(const std::string &_filename) {
    /* do Nothing. Not used */
    Q_UNUSED(_filename);
}

