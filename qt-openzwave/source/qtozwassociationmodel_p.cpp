//-----------------------------------------------------------------------------
//
//	qtozwassocationmodel_p.cpp
//
//	Association Groups Model - Used Internally to Manage the Model
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

#include <QBitArray>
#include "qtozw_logging.h"
#include "qtozwassociationmodel_p.h"

QTOZW_Associations_internal::QTOZW_Associations_internal(QObject *parent)
    : QTOZW_Associations(parent)
{

}

void QTOZW_Associations_internal::addGroup(quint8 _nodeID, quint8 _groupIDX) {
    if (getassocationRow(_nodeID, _groupIDX) != -1) {
        qCWarning(associationModel) << "addGroup: Group " << _groupIDX << "already exists on node " << _nodeID;
        return;
    }
    QMap<associationColumns, QVariant> newNode;
    newNode[associationColumns::NodeID] = _nodeID;
    newNode[associationColumns::GroupID] = _groupIDX;
    newNode[associationColumns::Members] = QStringList();
    QBitArray flags(static_cast<int>(associationFlags::FlagCount));
    newNode[associationColumns::Flags] = flags;

    this->beginInsertRows(QModelIndex(), this->rowCount(QModelIndex()), this->rowCount(QModelIndex()));
    this->m_associationData[this->rowCount(QModelIndex())] = newNode;
    this->endInsertRows();

    return;
}
void QTOZW_Associations_internal::setGroupData(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationColumns column, QVariant data) {
    switch (column) {
        case NodeID:
        case GroupID:
        case Members:
        case Flags:
        {
            qCWarning(associationModel) << "Can Not Set Group Data for " << static_cast<associationColumns>(column);
            return;
        }
        default:
            break;
    }

    qint32 row = getassocationRow(_nodeID, _groupIDX);
    if (row == -1) {
        qCWarning(associationModel) << "setGroupData: Can't Find Group " << _groupIDX << "on node " << _nodeID;
        return;
    }
    this->m_associationData[row][column] = data;
    QVector<int> roles;
    roles << Qt::DisplayRole;
    this->dataChanged(this->createIndex(row, column), this->createIndex(row, column), roles);
}
void QTOZW_Associations_internal::setGroupFlags(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationFlags _flags, bool _value) {
    qint32 row = getassocationRow(_nodeID, _groupIDX);
    if (row == -1) {
        qCWarning(associationModel) << "setGroupFlags: Can't Find Group " << _groupIDX << "on node " << _nodeID;
        return;
    }
    QBitArray flags(static_cast<int>(associationFlags::FlagCount));
    flags = this->m_associationData[row][associationColumns::Flags].toBitArray();
    flags.setBit(_flags, _value);
    this->m_associationData[row][associationColumns::Flags] = flags;
    QVector<int> roles;
    roles << Qt::DisplayRole;
    this->dataChanged(this->createIndex(row, associationColumns::Flags), this->createIndex(row, associationColumns::Flags), roles);

};

void QTOZW_Associations_internal::delNode(quint8 _nodeID) {
    QMap<qint32, QMap<QTOZW_Associations::associationColumns, QVariant> >::Iterator it;
    for (it = this->m_associationData.begin(); it != this->m_associationData.end();) {
        if (it.value()[associationColumns::NodeID] == _nodeID) {
            qCDebug(associationModel) << "Removing Node " << it.value()[associationColumns::NodeID] << it.key();
            this->beginRemoveRows(QModelIndex(), it.key(), it.key());
            it = this->m_associationData.erase(it);
            this->endRemoveRows();
        } else {
            it++;
        }
    }
}

void QTOZW_Associations_internal::addAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNode, quint8 _targetInstance) {
    qint32 row = getassocationRow(_nodeID, _groupIDX);
    if (row == -1) {
        qCWarning(associationModel) << "addAssciation: Can't Find Group " << _groupIDX << "for node " << _nodeID;
        return;
    }
    QString target;
    target.append(QString::number(_targetNode)).append(".").append(QString::number(_targetInstance));
    QStringList targetlist = this->m_associationData[row][associationColumns::Members].toStringList();
    if (targetlist.contains(target)) {
        qCWarning(associationModel) << "addAssociation: Target " << target << "already exists in Group " << _groupIDX << "for node " << _nodeID;
        return;
    }
    targetlist << target;
    this->m_associationData[row][associationColumns::Members] = targetlist;
    QVector<int> roles;
    roles << Qt::DisplayRole;
    this->dataChanged(this->createIndex(row, associationColumns::Members), this->createIndex(row, associationColumns::Members), roles);
}

void QTOZW_Associations_internal::delAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNode, quint8 _targetInstance) {
    qint32 row = getassocationRow(_nodeID, _groupIDX);
    if (row == -1) {
        qCWarning(associationModel) << "delAssciation: Can't Find Group " << _groupIDX << "for node " << _nodeID;
        return;
    }
    QString target;
    target.append(QString::number(_targetNode)).append(".").append(QString::number(_targetInstance));
    QStringList targetlist = this->m_associationData[row][associationColumns::Members].toStringList();
    if (!targetlist.contains(target)) {
        qCWarning(associationModel) << "delAssociation: Target " << target << "does not exist in Group " << _groupIDX << "for node " << _nodeID;
        return;
    }
    targetlist.removeAll(target);
    this->m_associationData[row][associationColumns::Members] = targetlist;
    QVector<int> roles;
    roles << Qt::DisplayRole;
    this->dataChanged(this->createIndex(row, associationColumns::Members), this->createIndex(row, associationColumns::Members), roles);
}

bool QTOZW_Associations_internal::findAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNode, quint8 _targetInstance) {
    qint32 row = getassocationRow(_nodeID, _groupIDX);
    if (row == -1) {
        qCWarning(associationModel) << "delAssciation: Can't Find Group " << _groupIDX << "for node " << _nodeID;
        return false;
    }
    QString target;
    target.append(QString::number(_targetNode)).append(".").append(QString::number(_targetInstance));
    QStringList targetlist = this->m_associationData[row][associationColumns::Members].toStringList();
    if (!targetlist.contains(target)) {
        return false;
    }
    return true;

}

void QTOZW_Associations_internal::resetModel() {
    this->beginRemoveRows(QModelIndex(), 0, this->m_associationData.count());
    this->m_associationData.clear();
    this->endRemoveRows();
}
