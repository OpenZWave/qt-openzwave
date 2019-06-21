#ifndef QTOZW_LOG_P_H
#define QTOZW_LOG_P_H
#include "qt-openzwave/qtozwlog.h"
#include <platform/Log.h>


class QTOZW_Log_Internal : public QTOZW_Log, public OpenZWave::i_LogImpl
{
    Q_OBJECT
public:
    QTOZW_Log_Internal(QObject *parent);
    /* ozw i_LogImpl Methods */
    virtual void Write(OpenZWave::LogLevel _level, uint8 const _nodeId, char const* _format, va_list _args);
    virtual void QueueDump();
    virtual void QueueClear();
    virtual void SetLoggingState(OpenZWave::LogLevel _saveLevel, OpenZWave::LogLevel _queueLevel, OpenZWave::LogLevel _dumpTrigger);
    virtual void SetLogFileName(const std::string &_filename);

Q_SIGNALS:
    bool doInsertLogLine(quint8 node, QDateTime timestamp, QTOZW_Log::LogLevels level, QString msg);
private Q_SLOTS:
    bool insertLogLine(quint8 node, QDateTime timestamp, QTOZW_Log::LogLevels level, QString msg);

};

#endif // QTOZW_LOG_P_H
