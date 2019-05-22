#include <QDebug>
#include <QBitArray>
#include <QDataStream>


#include "qtozwvalueidmodel.h"
#include "qtozwmanager.h"


QDataStream & operator<<( QDataStream & dataStream, const QTOZW_ValueIDList & list )
{
    dataStream << list.values << list.labels << list.selectedItem;
    return dataStream;
}

QDataStream & operator>>(QDataStream & dataStream, QTOZW_ValueIDList & list) {
    dataStream >> list.values >> list.labels >> list.selectedItem;
    return dataStream;
}

QDataStream & operator<<( QDataStream & dataStream, const QTOZW_ValueIDBitSet & list )
{
    dataStream << list.values << list.mask << list.label << list.help;
    return dataStream;
}

QDataStream & operator>>(QDataStream & dataStream, QTOZW_ValueIDBitSet & list) {
    dataStream >> list.values >> list.mask >> list.label >> list.help;
    return dataStream;
}


QTOZW_ValueIds::QTOZW_ValueIds(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int QTOZW_ValueIds::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return this->m_valueData.count();
}
int QTOZW_ValueIds::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return ValueIdColumns::ValueIdCount;
}
QVariant QTOZW_ValueIds::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QMap<ValueIdColumns, QVariant> node = this->m_valueData[index.row()];
        if (node.size() == 0) {
            qWarning() << "data: Cant find any Node on Row " << index.row();
            return QVariant();
        }
        return node[static_cast<ValueIdColumns>(index.column())];
    }
    return QVariant();

}
QVariant QTOZW_ValueIds::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<ValueIdColumns>(section)) {
            case Label:
                return tr("Label");

            case Value:
                return tr("Value");

            case Units:
                return tr("Units");

            case Min:
                return tr("Min");

            case Max:
                return tr("Max");

            case Type:
                return tr("Type");

            case Instance:
                return tr("Instance");

            case CommandClass:
                return tr("CommandClass");

            case Index:
                return tr("Index");

            case Node:
                return tr("Node");

            case Genre:
                return tr("Genre");

            case Help:
                return tr("Help");

            case ValueIDKey:
                return tr("ValueID Key");

            case ValueFlags:
                return tr("Flags");

            case ValueIdCount:
                return QVariant();
        }
    }
    return QVariant();
}
Qt::ItemFlags QTOZW_ValueIds::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    switch (static_cast<ValueIdColumns>(index.column())) {
        case Value:
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        default:
            return QAbstractTableModel::flags(index);
    }
}
bool QTOZW_ValueIds::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole) {
        return false;
    }
    switch (static_cast<ValueIdColumns>(index.column())) {
        case Value:
            if (this->m_valueData[index.row()][static_cast<ValueIdColumns>(index.column())] != value) {
                this->m_valueData[index.row()][static_cast<ValueIdColumns>(index.column())] = value;
                QVector<int> roles;
                roles << Qt::DisplayRole << QTOZW_UserRoles::ModelDataChanged;
                this->dataChanged(index, index, roles);
            }
        break;
        default:
            return false;
    }
    return true;
}

QVariant QTOZW_ValueIds::getValueData(uint64_t _vidKey, ValueIdColumns _column) {
    int32_t row = this->getValueRow(_vidKey);
    if (row >= 0)
        return this->m_valueData[row][_column];
    qWarning() << "Can't find ValueData for ValueID " << _vidKey;
    return QVariant();
}

int32_t QTOZW_ValueIds::getValueRow(uint64_t _vidKey) {
    if (this->m_valueData.count() == 0) {
        return -1;
    }
    QMap<int32_t, QMap<ValueIdColumns, QVariant> >::iterator it;
    for (it = m_valueData.begin(); it != m_valueData.end(); ++it) {
        QMap<ValueIdColumns, QVariant> node = it.value();
        if (node.value(ValueIdColumns::ValueIDKey) == _vidKey) {
            return it.key();
        }
    }
    qWarning() << "Can't Find ValueID " << _vidKey << " in valueData";
    return -1;
}


QTOZW_ValueIds_internal::QTOZW_ValueIds_internal(QObject *parent)
    : QTOZW_ValueIds(parent)
{

}

void QTOZW_ValueIds_internal::addValue(uint64_t _vidKey)
{
    if (this->getValueRow(_vidKey) >= 0) {
        qWarning() << "ValueID " << _vidKey << " Already Exists";
        return;
    }
//    QMap<int32_t, QMap<ValueIdColumns, QVariant> >
    QMap<ValueIdColumns, QVariant> newValue;
    newValue[QTOZW_ValueIds::ValueIDKey] = _vidKey;
    QBitArray flags(static_cast<int>(ValueIDFlags::FlagCount));
    newValue[QTOZW_ValueIds::ValueFlags] = flags;

    this->beginInsertRows(QModelIndex(), this->rowCount(QModelIndex()), this->rowCount(QModelIndex()));
    this->m_valueData[this->rowCount(QModelIndex())] = newValue;
    this->endInsertRows();
}

void QTOZW_ValueIds_internal::setValueData(uint64_t _vidKey, QTOZW_ValueIds::ValueIdColumns column, QVariant data)
{
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qWarning() << "setValueData: Value " << _vidKey << " does not exist";
        return;
    }
    if (this->m_valueData[row][column] != data) {
        this->m_valueData[row][column] = data;
        this->dataChanged(this->createIndex(row, column), this->createIndex(row, column));
    }
}

void QTOZW_ValueIds_internal::setValueFlags(uint64_t _vidKey, QTOZW_ValueIds::ValueIDFlags _flags, bool _value)
{
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qWarning() << "setValueFlags: Value " << _vidKey << " does not exist";
        return;
    }
    QBitArray flag = this->m_valueData[row][QTOZW_ValueIds::ValueFlags].toBitArray();
    if (flag.at(_flags) != _value) {
        flag.setBit(_flags, _value);
        this->m_valueData[row][QTOZW_ValueIds::ValueFlags] = flag;
        this->dataChanged(this->createIndex(row, QTOZW_ValueIds::ValueFlags), this->createIndex(row, QTOZW_ValueIds::ValueFlags));

    }
}

void QTOZW_ValueIds_internal::delValue(uint64_t _vidKey) {
    QMap<int32_t, QMap<ValueIdColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<ValueIdColumns, QVariant> > newValueMap;
    int32_t newrow = 0;
    for (it = this->m_valueData.begin(); it != this->m_valueData.end(); ++it) {
        if (it.value()[QTOZW_ValueIds::ValueIdColumns::ValueIDKey] == _vidKey) {
            qDebug() << "Removing Value " << it.value()[QTOZW_ValueIds::ValueIdColumns::Label] << it.key();
            this->beginRemoveRows(QModelIndex(), it.key(), it.key());
            this->m_valueData.erase(it);
            this->endRemoveRows();
            continue;
        } else {
            newValueMap[newrow] = it.value();
            newrow++;
        }
    }
    this->m_valueData.swap(newValueMap);
}

void QTOZW_ValueIds_internal::delNodeValues(uint8_t _node) {
    QMap<int32_t, QMap<ValueIdColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<ValueIdColumns, QVariant> > newValueMap;
    int32_t newrow = 0;
    for (it = this->m_valueData.begin(); it != this->m_valueData.end(); ++it) {
        if (it.value()[QTOZW_ValueIds::ValueIdColumns::Node] == _node) {
            qDebug() << "Removing Value " << it.value()[QTOZW_ValueIds::ValueIdColumns::Label] << it.key();
            this->beginRemoveRows(QModelIndex(), it.key(), it.key());
            this->m_valueData.erase(it);
            this->endRemoveRows();
            continue;
        } else {
            newValueMap[newrow] = it.value();
            newrow++;
        }
    }
    this->m_valueData.swap(newValueMap);
}

void QTOZW_ValueIds_internal::resetModel() {
    this->beginRemoveRows(QModelIndex(), 0, this->m_valueData.count());
    this->m_valueData.clear();
    this->endRemoveRows();
}
