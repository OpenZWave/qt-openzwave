#ifndef QTOZWLOGGING_H
#define QTOZWLOGGING_H

#include <QObject>
#include <QAbstractTableModel>
#include <QDateTime>


class QTOZW_Logging : public QAbstractTableModel {
    Q_OBJECT
public:
    enum LoggingColumns {
        TimeStamp,
        Node,
        Level,
        Message,
        Count
    };
    Q_ENUM(LoggingColumns)
    enum LoggingLevels {
        Always,
        Fatal,
        Error,
        Warning,
        Alert,
        Info,
        Debug,
        StreamDetail,
        Internal,
        LogLevelCount
    };
    Q_ENUM(LoggingLevels)
    struct QTOZW_LogEntry {
        QDateTime s_time;
        quint8 s_node;
        LoggingColumns s_level;
        QString s_msg;
    };


    QTOZW_Logging(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
protected:
    QTOZW_LogEntry getLogData(int);

    QVector<QTOZW_LogEntry> m_logData;
};



#endif // QTOZWLOGGING_H
