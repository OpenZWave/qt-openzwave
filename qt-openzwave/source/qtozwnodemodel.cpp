//-----------------------------------------------------------------------------
//
//	qtozwnodemodel.cpp
//
//	Nodes Exposed as a QAbstractTableModel
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
#include "qt-openzwave/qtozwmanager.h"
#include "qt-openzwave/qtozwnodemodel.h"
#include "qt-openzwave/qtopenzwave.h"


QTOZW_Nodes::QTOZW_Nodes(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int QTOZW_Nodes::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return this->m_nodeData.count();
}
int QTOZW_Nodes::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return QTOZW_Nodes::NodeCount;
}
QVariant QTOZW_Nodes::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QMap<NodeColumns, QVariant> node = this->m_nodeData.at(index.row());
        if (node.size() == 0) {
            qCWarning(nodeModel) << "data: Cant find any Node on Row " << index.row();
            return QVariant();
        }
        qDebug() << node[static_cast<NodeColumns>(index.column())]; 
        return node[static_cast<NodeColumns>(index.column())];
    }
    return QVariant();

}
QVariant QTOZW_Nodes::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (static_cast<NodeColumns>(section)) {
            case NodeID:
                return tr("NodeID");

            case NodeName:
                return tr("Node Name");

            case NodeLocation:
                return tr("Location");

            case NodeManufacturerName:
                return tr("Manufacturer");

            case NodeProductName:
                return tr("Product");

            case NodeBasicString:
                return tr("Basic Type");

            case NodeBasic:
                return tr("Basic Type");

            case NodeGenericString:
                return tr("Generic Type");

            case NodeGeneric:
                return tr("Generic Type");

            case NodeSpecificString:
                return tr("Specific Type");

            case NodeSpecific:
                return tr("Specific Type");

            case NodeDeviceTypeString:
                return tr("Device Type");

            case NodeDeviceType:
                return tr("Device Type");

            case NodeRole:
                return tr("Node Role");

            case NodeRoleString:
                return tr("Node Role");

            case NodePlusType:
                return tr("Node Plus Type");

            case NodePlusTypeString:
                return tr("Node Plus Type");

            case NodeQueryStage:
                return tr("Query Stage");

            case NodeManufacturerID:
                return tr("Manufacturer ID");

            case NodeProductID:
                return tr("Product ID");

            case NodeProductType:
                return tr("Product Type");

            case NodeBaudRate:
                return tr("Node Baud Rate");

            case NodeVersion:
                return tr("Node Version");

            case NodeGroups:
                return tr("Node Groups Supported");

            case NodeFlags:
                return tr("Node Flags");

            case NodeCount:
                return QVariant();
        }
    }
    return QVariant();
}
Qt::ItemFlags QTOZW_Nodes::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    switch (static_cast<NodeColumns>(index.column())) {
        case NodeName:
        case NodeLocation:
            return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
        default:
            return QAbstractTableModel::flags(index);
    }
}
bool QTOZW_Nodes::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole) {
        return false;
    }
    switch (static_cast<NodeColumns>(index.column())) {
        case NodeName:
        case NodeLocation:
            if (this->m_nodeData.at(index.row())[static_cast<NodeColumns>(index.column())] != value) {
                this->m_nodeData[index.row()][static_cast<NodeColumns>(index.column())] = value;
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

QVariant QTOZW_Nodes::getNodeData(quint8 _node, QTOZW_Nodes::NodeColumns _column) {
    int32_t row = this->getNodeRow(_node);
    if (row >= 0)
        return this->m_nodeData.at(row)[_column];
    qCWarning(nodeModel) << "Can't find NodeData for Node " << _node;
    return QVariant();
}

int32_t QTOZW_Nodes::getNodeRow(quint8 _node) {
    if (this->m_nodeData.count() == 0) {
        return -1;
    }
    QVector< QMap<NodeColumns, QVariant> >::iterator it;
    int i = 0;
    for (it = m_nodeData.begin(); it != m_nodeData.end(); ++it) {
        QMap<NodeColumns, QVariant> node = (*it);
        if (node.value(QTOZW_Nodes::NodeID) == _node) {
            return i;
        }
        i++;
    }
    //qCWarning(nodeModel) << "Can't Find NodeID " << _node << " in NodeData";
    return -1;
}
