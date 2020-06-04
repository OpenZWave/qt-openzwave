//-----------------------------------------------------------------------------
//
//	qtozwlog.h
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

#ifndef QTOZWLOGGING_H
#define QTOZWLOGGING_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDateTime>

#include "qt-openzwave/qtozwreplica.h"
#include "qt-openzwave/rep_qtozwlog_replica.h"

class QTOZWLog_Internal;
class QTOZWLogModel;
class QTOPENZWAVESHARED_EXPORT QTOZWLog : public QTOZWReplicaBase {
    friend class QTOZWLogModel;
    Q_OBJECT
    public:
        struct QTOZW_LogEntry {
            QString s_msg;
            QDateTime s_time;
            quint8 s_node;
            LogLevels::Level s_level;
        };


        QTOZWLog(ConnectionType::Type connectionType, QObject *parent = nullptr);
        ~QTOZWLog();

        bool initilizeBase() override;
        bool initilizeSource(QRemoteObjectHost *m_sourceNode) override;
        bool initilizeReplica(QRemoteObjectNode *m_replicaNode) override;
        QVector<QTOZWLog::QTOZW_LogEntry> getLogEntries();

    Q_SIGNALS:
        void newLogLine(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);
        void syncronizedLogLine(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);
        void logCleared();

    public Q_SLOTS:
        quint32 getLogCount();
        bool syncroniseLogs();

    protected:
        QVector<QTOZW_LogEntry> m_logData;
        quint32 m_maxLogLength;

    private Q_SLOTS:
        void insertLogLine(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);
        void syncLogMessages(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);
    private:
        void connectSignals() override;

        QTOZWLog_Internal *d_ptr_internal;
        QTOZWLogReplica *d_ptr_replica;
};

class QTOPENZWAVESHARED_EXPORT QTOZWLogModel : public QAbstractTableModel {
    Q_OBJECT
    public:
        enum LogColumns {
            TimeStamp,
            Node,
            Level,
            Message,
            Count
        };
        Q_ENUM(LogColumns)

        QTOZWLogModel (QTOZWLog *qtozwlog, QObject *parent = nullptr);
        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
        Qt::ItemFlags flags(const QModelIndex &index) const override;
    public Q_SLOTS:
        bool insertLogMessage(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);
        bool syncLogMessage(QDateTime time, LogLevels::Level level, quint8 s_node, QString s_msg);

        void resetModel();
        QTOZWLog::QTOZW_LogEntry getLogData(int) const;
    private:
        QTOZWLog *m_qtozwlog;
};

#endif // QTOZWLOGGING_H
