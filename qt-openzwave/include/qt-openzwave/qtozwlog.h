#ifndef QTOZWLOGGING_H
#define QTOZWLOGGING_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDateTime>


class QTOZW_Log : public QAbstractTableModel {
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
    enum LogLevels {
        Always,
        Fatal,
        Error,
        Warning,
        Alert,
        Info,
        Detail,
        Debug,
        StreamDetail,
        Internal,
        LogLevelCount
    };
    Q_ENUM(LogLevels)
    struct QTOZW_LogEntry {
        QString s_msg;
        QDateTime s_time;
        quint8 s_node;
        LogLevels s_level;
    };


    QTOZW_Log(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
protected:
    QTOZW_LogEntry getLogData(int) const;

    QVector<QTOZW_LogEntry> m_logData;
    quint32 m_maxLogLength;
};



#endif // QTOZWLOGGING_H
