#include "include/qt-openzwave/qtozwlogging.h"
#include "include/qtozw_logging.h"

QTOZW_Logging::QTOZW_Logging(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int QTOZW_Logging::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return this->m_logData.count();
}
int QTOZW_Logging::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return LoggingColumns::Count;
}
QVariant QTOZW_Logging::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {


    }
    return QVariant();

}
QVariant QTOZW_Logging::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<LoggingColumns>(section)) {
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
Qt::ItemFlags QTOZW_Logging::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractTableModel::flags(index);
}

QTOZW_Logging::QTOZW_LogEntry QTOZW_Logging::getLogData(int pos) {
    if (this->m_logData.size() <= pos)
        return this->m_logData.at(pos);
    qCWarning(logModel) << "Can't find LogEntry at " << pos;
    return QTOZW_LogEntry();
}
