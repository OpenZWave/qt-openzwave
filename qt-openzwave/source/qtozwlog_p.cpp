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
    QTOZW_Log::LogLevels qtozwlevel;
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
            qCCritical(libopenzwave) << "Warning - Node:" << _nodeId << lineBuf;
            break;
        }
        case OpenZWave::LogLevel_Alert: {
            qtozwlevel = LogLevels::Alert;
            qCCritical(libopenzwave) << "Alert - Node:" << _nodeId << lineBuf;
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

