//-----------------------------------------------------------------------------
//
//	qtozwassocationmodel.cpp
//
//	Association Groups Exposed as a QAbstractTableModel
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
#include "qtozw_logging.h"
#include "qt-openzwave/qtozwassociationmodel.h"
#include "qt-openzwave/qtopenzwave.h"


QTOZW_Associations::QTOZW_Associations(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int QTOZW_Associations::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return this->m_associationData.count();
}
int QTOZW_Associations::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return QTOZW_Associations::associationColumns::Count;
}
QVariant QTOZW_Associations::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QMap<associationColumns, QVariant> node = this->m_associationData[index.row()];
        if (node.size() == 0) {
            qCWarning(associationModel) << "data: Cant find any Node on Row " << index.row();
            return QVariant();
        }
        return node[static_cast<associationColumns>(index.column())];
    }
    return QVariant();

}

QVariant QTOZW_Associations::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<associationColumns>(section)) {
            case NodeID:
                return tr("NodeID");

            case GroupID:
                return tr("Group ID");

            case GroupName:
                return tr("Group Name");

            case GroupHelp:
                return tr("Help");

            case MaxAssocations:
                return tr("Max Assocations");

            case Members:
                return tr("Members");

            case Flags:
                return tr("Flags");

            case Count:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant QTOZW_Associations::getassocationData(quint8 _node, quint8 _groupIDX, associationColumns _column) {
    int32_t row = this->getassocationRow(_node, _groupIDX);
    if (row >= 0)
        return this->m_associationData[row][_column];
    qCWarning(associationModel) << "Can't find Group " << _groupIDX << " for node " << _node << " in m_associationData";
    return QVariant();
}

int32_t QTOZW_Associations::getassocationRow(quint8 _node, quint8 _groupIDX) {
    if (this->m_associationData.count() == 0) {
        return -1;
    }
    QMap<int32_t, QMap<associationColumns, QVariant> >::iterator it;
    for (it = m_associationData.begin(); it != m_associationData.end(); ++it) {
        QMap<associationColumns, QVariant> group = it.value();
        if (group.value(associationColumns::NodeID) == _node) {
            if (group.value(associationColumns::GroupID) == _groupIDX) {
                qCDebug(associationModel) << "getAssociationRow: Found Association at Row " << it.key();
                return it.key();
            }
        }
    }
    qCWarning(associationModel) << "Can't Find Group " << _groupIDX << " for node " << _node << " in m_associationData";
    return -1;
}
