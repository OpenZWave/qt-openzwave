#include "qtozw_logging.h"
#include "qtozwvalueidmodel_p.h"

QTOZW_ValueIds_internal::QTOZW_ValueIds_internal(QObject *parent)
    : QTOZW_ValueIds(parent)
{

}

void QTOZW_ValueIds_internal::addValue(quint64 _vidKey)
{
    if (this->getValueRow(_vidKey) >= 0) {
        qCWarning(valueModel) << "ValueID " << _vidKey << " Already Exists";
        return;
    }
    QMap<ValueIdColumns, QVariant> newValue;
    newValue[QTOZW_ValueIds::ValueIDKey] = _vidKey;
    QBitArray flags(static_cast<int>(ValueIDFlags::FlagCount));
    newValue[QTOZW_ValueIds::ValueFlags] = flags;

    this->beginInsertRows(QModelIndex(), this->rowCount(QModelIndex()), this->rowCount(QModelIndex()));
    this->m_valueData[this->rowCount(QModelIndex())] = newValue;
    this->endInsertRows();
}

void QTOZW_ValueIds_internal::setValueData(quint64 _vidKey, QTOZW_ValueIds::ValueIdColumns column, QVariant data)
{
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qCWarning(valueModel) << "setValueData: Value " << _vidKey << " does not exist";
        return;
    }
    if (this->m_valueData[row][column] != data) {
        this->m_valueData[row][column] = data;
        this->dataChanged(this->createIndex(row, column), this->createIndex(row, column));
    }
}

void QTOZW_ValueIds_internal::setValueFlags(quint64 _vidKey, QTOZW_ValueIds::ValueIDFlags _flags, bool _value)
{
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qCWarning(valueModel) << "setValueFlags: Value " << _vidKey << " does not exist";
        return;
    }
    if (this->m_valueData[row][QTOZW_ValueIds::ValueFlags].toBitArray().at(_flags) != _value) {
        QBitArray flags = this->m_valueData[row][QTOZW_ValueIds::ValueFlags].value<QBitArray>();
        flags.setBit(_flags, _value);
        this->m_valueData[row][QTOZW_ValueIds::ValueFlags] = flags;
        this->dataChanged(this->createIndex(row, QTOZW_ValueIds::ValueFlags), this->createIndex(row, QTOZW_ValueIds::ValueFlags));
    }
}

void QTOZW_ValueIds_internal::delValue(quint64 _vidKey) {
    QMap<int32_t, QMap<ValueIdColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<ValueIdColumns, QVariant> > newValueMap;
    int32_t newrow = 0;
    for (it = this->m_valueData.begin(); it != this->m_valueData.end(); ++it) {
        if (it.value()[QTOZW_ValueIds::ValueIdColumns::ValueIDKey] == _vidKey) {
            qCDebug(valueModel) << "Removing Value " << it.value()[QTOZW_ValueIds::ValueIdColumns::Label] << it.key();
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

void QTOZW_ValueIds_internal::delNodeValues(quint8 _node) {
    QMap<int32_t, QMap<ValueIdColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<ValueIdColumns, QVariant> > newValueMap;
    qint32 newrow = 0;
    for (it = this->m_valueData.begin(); it != this->m_valueData.end(); ++it) {
        if (it.value()[QTOZW_ValueIds::ValueIdColumns::Node] == _node) {
            qCDebug(valueModel) << "Removing Value " << it.value()[QTOZW_ValueIds::ValueIdColumns::Label] << it.key();
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





QString BitSettoQString(QBitArray ba) {
    QString result;
    for (int i = ba.size()-1; i >= 0 ; --i) {
        if (ba.testBit(i))
            result[ba.size() - i] = '1';
        else
            result[ba.size() - i] = '0';
    }
    return result;
#if 0
    result.prepend("0b");
    return result;
#endif
}

quint32 BitSettoInteger(QBitArray ba) {
    quint32 value = 0;
    for (int i = 0; i <= ba.size()-1; ++i) {
        value += (quint32)((ba.at(i) ? 1 : 0) << i);
    }
    return value;
}
