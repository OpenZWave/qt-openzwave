//-----------------------------------------------------------------------------
//
//	qtozwvalueidmodel.cpp
//
//	Value's exposed as a QAstractTableModel
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

#include <QDebug>
#include <QBitArray>
#include <QDataStream>

#include "qtozw_logging.h"
#include "qt-openzwave/qtozwvalueidmodel.h"
#include "qt-openzwave/qtozwmanager.h"


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
    if (parent.isValid())
        return 0;
    return this->m_valueData.count();
}
int QTOZW_ValueIds::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return ValueIdColumns::ValueIdCount;
}
QVariant QTOZW_ValueIds::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    //qDebug() << "data" << index.row() << this->rowCount(index.parent());
    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        //qDebug() << "Doing Display Data";
        QMap<ValueIdColumns, QVariant> value = this->m_valueData.at(index.row());
        if (value.size() == 0) {
            qCWarning(valueModel) << "data: Cant find any Node on Row " << index.row();
            return QVariant();
        }
        //qDebug() << value[ValueIdColumns::ValueIDKey] << QMetaEnum::fromType<ValueIdColumns>().valueToKey(index.column()) << value;
        return value[static_cast<ValueIdColumns>(index.column())];
    }
    if (role == Qt::ToolTipRole) {
        QMap<ValueIdColumns, QVariant> value = this->m_valueData.at(index.row());
        if (value.size() == 0) {
            qCWarning(valueModel) << "data: Cant find any Node on Row " << index.row();
            return QVariant();
        }
        return value[static_cast<ValueIdColumns>(ValueIdColumns::Help)];
    }
    //qDebug() << "Returning Invalid";
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
        return Qt::NoItemFlags;
    switch (static_cast<ValueIdColumns>(index.column())) {
        case Value: {
            QBitArray flags = index.sibling(index.row(), ValueIdColumns::ValueFlags).data().value<QBitArray>();
            if (flags.at(ValueIDFlags::ReadOnly) == true) {
                return QAbstractTableModel::flags(index);
            } else {
                return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
            }
        }
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
            if (this->m_valueData.at(index.row())[static_cast<ValueIdColumns>(index.column())] != value) {
                this->m_valueData.value(index.row())[static_cast<ValueIdColumns>(index.column())] = value;
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

QVariant QTOZW_ValueIds::getValueData(quint64 _vidKey, ValueIdColumns _column) {
    int32_t row = this->getValueRow(_vidKey);
    if (row != -1)
        return this->m_valueData.at(row)[_column];
    qCWarning(valueModel) << "Can't find ValueData for ValueID " << _vidKey;
    return QVariant();
}

int32_t QTOZW_ValueIds::getValueRow(quint64 _vidKey) {
    if (this->m_valueData.count() == 0) {
        return -1;
    }
    //qDebug() << "ValueModel Size:" << m_valueData.count();
    QVector< QMap<ValueIdColumns, QVariant> >::iterator it;
    int i = 0;
    for (it = m_valueData.begin(); it != m_valueData.end(); ++it) {
        QMap<ValueIdColumns, QVariant> node = (*it);
        //qDebug() << node.value(ValueIdColumns::ValueIDKey) << _vidKey;
        if (node.value(ValueIdColumns::ValueIDKey) == _vidKey) {
            return i;
        }
        i++;
    }
    //qCWarning(valueModel) << "Can't Find ValueID " << _vidKey << " in valueData";
    return -1;
}
