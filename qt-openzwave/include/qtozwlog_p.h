//-----------------------------------------------------------------------------
//
//	qtozwlog_p.h
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

#ifndef QTOZW_LOG_P_H
#define QTOZW_LOG_P_H
#include "qt-openzwave/qtopenzwave.h"
#include "qt-openzwave/qtozwlog.h"
#include "qt-openzwave/rep_qtozwlog_source.h"
#include <platform/Log.h>


class QTOZWLog_Internal : public QTOZWLogSimpleSource, public OpenZWave::i_LogImpl
{
    Q_OBJECT
    public:
        QTOZWLog_Internal(QObject *parent = nullptr);
        ~QTOZWLog_Internal();

        virtual void Write(OpenZWave::LogLevel _level, uint8 const _nodeId, char const* _format, va_list _args) override;
        virtual void QueueDump() override;
        virtual void QueueClear() override;
        virtual void SetLoggingState(OpenZWave::LogLevel _saveLevel, OpenZWave::LogLevel _queueLevel, OpenZWave::LogLevel _dumpTrigger) override;
        virtual void SetLogFileName(const std::string &_filename) override;
        virtual quint32 getLogCount() override;
        virtual bool syncroniseLogs(quint32 records) override;
    Q_SIGNALS:
        void sendLogLine(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);

    private:
        struct QTOZW_LogEntry {
            QString s_msg;
            QDateTime s_time;
            quint8 s_node;
            LogLevels::Level s_level;
        };
        QVector<QTOZW_LogEntry> m_logData;
        quint32 m_maxLogLength;
};

#if 0
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
#endif
#endif // QTOZW_LOG_P_H
