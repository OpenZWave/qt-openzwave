#include "include/qt-openzwave/qtozwlog.h"
#include "include/qtozw_logging.h"

QTOZW_Log::QTOZW_Log(QObject *parent)
    : QAbstractTableModel(parent),
      m_maxLogLength(100000)
{
}

int QTOZW_Log::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return this->m_logData.count();
}
int QTOZW_Log::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return LogColumns::Count;
}
QVariant QTOZW_Log::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QTOZW_LogEntry le = this->getLogData(index.row()-1);
        if (!le.s_time.isValid()) {
            qCWarning(logModel) << "QTOZW_Log::data asked for a invalid row " << index.row() << " size: " << rowCount(QModelIndex());
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
QVariant QTOZW_Log::headerData(int section, Qt::Orientation orientation, int role) const {
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
Qt::ItemFlags QTOZW_Log::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractTableModel::flags(index);
}

QTOZW_Log::QTOZW_LogEntry QTOZW_Log::getLogData(int pos) const {
    if (this->m_logData.size() >= pos)
        return this->m_logData.at(pos+1);
    qCWarning(logModel) << "Can't find LogEntry at " << pos << " size:" << this->m_logData.size();
    return QTOZW_LogEntry();
}
