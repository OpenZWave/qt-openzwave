//-----------------------------------------------------------------------------
//
//	qtozwvalueidmodel_p.cpp
//
//	ValueID Model - Internal Class to Manage the Model
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
    this->m_valueData.push_back(newValue);
    this->endInsertRows();
}

void QTOZW_ValueIds_internal::setValueData(quint64 _vidKey, QTOZW_ValueIds::ValueIdColumns column, QVariant data, bool transaction)
{
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qCWarning(valueModel) << "setValueData: Value " << _vidKey << " does not exist";
        return;
    }
    if (this->m_valueData.at(row)[column] != data) {
        this->m_valueData[row][column] = data;
        QVector<int> roles;
        roles << Qt::DisplayRole;
        if (!transaction) this->dataChanged(this->createIndex(row, column), this->createIndex(row, column), roles);
        //qDebug() << "Setting Data " << _vidKey << row <<  QMetaEnum::fromType<ValueIdColumns>().valueToKey(column) << data << this->m_valueData.value(row)[column] << this->m_valueData.at(row)[ValueIdColumns::ValueIDKey];
    }
}

void QTOZW_ValueIds_internal::setValueFlags(quint64 _vidKey, QTOZW_ValueIds::ValueIDFlags _flags, bool _value, bool transaction)
{
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qCWarning(valueModel) << "setValueFlags: Value " << _vidKey << " does not exist";
        return;
    }
    if (this->m_valueData.at(row)[QTOZW_ValueIds::ValueFlags].toBitArray().at(_flags) != _value) {
        QBitArray flags = this->m_valueData.value(row)[QTOZW_ValueIds::ValueFlags].value<QBitArray>();
        flags.setBit(_flags, _value);
        this->m_valueData.value(row)[QTOZW_ValueIds::ValueFlags] = flags;
        QVector<int> roles;
        roles << Qt::DisplayRole;
        if (!transaction) this->dataChanged(this->createIndex(row, QTOZW_ValueIds::ValueFlags), this->createIndex(row, QTOZW_ValueIds::ValueFlags), roles);
    }
}

void QTOZW_ValueIds_internal::delValue(quint64 _vidKey) {
    QVector< QMap<ValueIdColumns, QVariant> >::iterator it;
    int i = 0;
    for (it = this->m_valueData.begin(); it != this->m_valueData.end();) {
        if ((*it)[QTOZW_ValueIds::ValueIdColumns::ValueIDKey] == _vidKey) {
            qCDebug(valueModel) << "delValue: Removing Value " << (*it)[QTOZW_ValueIds::ValueIdColumns::Label] << (*it)[QTOZW_ValueIds::ValueIdColumns::ValueIDKey] << i;
            this->beginRemoveRows(QModelIndex(), i, i);
            it = this->m_valueData.erase(it);
            this->endRemoveRows();
            continue;
        } else {
            it++;
        }
        i++;
    }
}

void QTOZW_ValueIds_internal::delNodeValues(quint8 _node) {
    QVector< QMap<ValueIdColumns, QVariant> >::iterator it;
    int i = 0;
    for (it = this->m_valueData.begin(); it != this->m_valueData.end();) {
        if ((*it)[QTOZW_ValueIds::ValueIdColumns::Node] == _node) {
            qCDebug(valueModel) << "delNodeValue: Removing Value " << (*it)[QTOZW_ValueIds::ValueIdColumns::Label] << (*it)[QTOZW_ValueIds::ValueIdColumns::ValueIDKey] << i;
            this->beginRemoveRows(QModelIndex(), i, i);
            it = this->m_valueData.erase(it);
            this->endRemoveRows();
        } else {
            it++;
        }
        i++;
    }
}

void QTOZW_ValueIds_internal::resetModel() {
    this->beginRemoveRows(QModelIndex(), 0, this->m_valueData.count());
    this->m_valueData.clear();
    this->endRemoveRows();
}

void QTOZW_ValueIds_internal::finishTransaction(quint64 _vidKey) {
    int row = this->getValueRow(_vidKey);
    if (row == -1) {
        qCWarning(valueModel) << "setValueFlags: Value " << _vidKey << " does not exist";
        return;
    }
    this->dataChanged(this->createIndex(row, 0), this->createIndex(row, QTOZW_ValueIds::ValueIdColumns::ValueIdCount -1));
}

QVariant QTOZW_ValueIds_internal::getValueData(quint64 node, ValueIdColumns column) {
    return QTOZW_ValueIds::getValueData(node, column);
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
