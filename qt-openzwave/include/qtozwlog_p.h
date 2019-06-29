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
