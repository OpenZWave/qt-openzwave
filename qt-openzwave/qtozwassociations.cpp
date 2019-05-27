#include <QDebug>
#include <QBitArray>
#include "qtozwassociations.h"
#include "qtopenzwave.h"


QTOZW_Associations::QTOZW_Associations(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int QTOZW_Associations::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return this->m_associationData.count();
}
int QTOZW_Associations::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
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
            qWarning() << "data: Cant find any Node on Row " << index.row();
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

            case MemberNodeID:
                return tr("Member");

            case MemberNodeInstance:
                return tr("Member Node Instance");

            case Flags:
                return tr("Flags");

            case Count:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant QTOZW_Associations::getassocationData(quint8 _node, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance, associationColumns _column) {
    int32_t row = this->getassocationRow(_node, _groupIDX, _targetNodeId, _targetNodeInstance);
    if (row >= 0)
        return this->m_associationData[row][_column];
    qWarning() << "Can't find Group " << _groupIDX << " for node " << _node << " with Target Node: " << _targetNodeId << ":" << _targetNodeInstance << " in m_associationData";
    return QVariant();
}

int32_t QTOZW_Associations::getassocationRow(quint8 _node, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance) {
    if (this->m_associationData.count() == 0) {
        return -1;
    }
    QMap<int32_t, QMap<associationColumns, QVariant> >::iterator it;
    for (it = m_associationData.begin(); it != m_associationData.end(); ++it) {
        QMap<associationColumns, QVariant> group = it.value();
        if (group.value(associationColumns::NodeID) == _node) {
            if (group.value(associationColumns::GroupID) == _groupIDX) {
                if (group.value(associationColumns::MemberNodeID) == _targetNodeId) {
                    if (group.value(associationColumns::MemberNodeInstance) == _targetNodeInstance) {
                        return it.key();
                    }
                }
            }
        }
    }
    qWarning() << "Can't Find Group " << _groupIDX << " for node " << _node << " with Target Node: " << _targetNodeId << ":" << _targetNodeInstance << " in m_associationData";
    return -1;
}




QTOZW_Associations_internal::QTOZW_Associations_internal(QObject *parent)
    : QTOZW_Associations(parent)
{

}

void QTOZW_Associations_internal::addAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance)
{
    if (this->getassocationRow(_nodeID, _groupIDX, _targetNodeId, _targetNodeInstance) >= 0) {
        qWarning() << "Association Group " << _groupIDX << " for node " << _nodeID << " with Target Node: " << _targetNodeId << ":" << _targetNodeInstance << " Already Exists";
        return;
    }
    QMap<associationColumns, QVariant> newNode;
    newNode[associationColumns::NodeID] = _nodeID;
    newNode[associationColumns::GroupID] = _groupIDX;
    newNode[associationColumns::MemberNodeID] = _targetNodeId;
    newNode[associationColumns::MemberNodeInstance] = _targetNodeInstance;
    if (this->m_defaultData.count(_nodeID) && this->m_defaultData[_nodeID].count(_groupIDX)) {
        QMap<QTOZW_Associations::associationColumns, QVariant>::iterator it;
        for (it = this->m_defaultData[_nodeID][_groupIDX].begin(); it != this->m_defaultData[_nodeID][_groupIDX].end(); ++it) {
            newNode[it.key()] = it.value();
        }
    }
    if (newNode[associationColumns::Flags].toBitArray().size() == 0) {
        QBitArray flags(static_cast<int>(associationColumns::Count));
        newNode[associationColumns::Flags] = flags;
    }
    this->beginInsertRows(QModelIndex(), this->rowCount(QModelIndex()), this->rowCount(QModelIndex()));
    this->m_associationData[this->rowCount(QModelIndex())] = newNode;
    this->endInsertRows();
}

void QTOZW_Associations_internal::delAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance) {
    QMap<int32_t, QMap<associationColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<associationColumns, QVariant> > newAssociationMap;
    int32_t newrow = 0;
    for (it = this->m_associationData.begin(); it != this->m_associationData.end(); ++it) {
        if (it.value()[associationColumns::NodeID] == _nodeID) {
            if (it.value()[associationColumns::GroupID] == _groupIDX) {
                if (it.value()[associationColumns::MemberNodeID] == _targetNodeId) {
                    if (it.value()[associationColumns::MemberNodeInstance] == _targetNodeInstance) {
                        qDebug() << "Removing Node " << it.value()[associationColumns::NodeID] << it.key();
                        this->beginRemoveRows(QModelIndex(), it.key(), it.key());
                        this->m_associationData.erase(it);
                        this->endRemoveRows();
                        continue;
                    }
                }
            }
        }
        newAssociationMap[newrow] = it.value();
        newrow++;
    }
    this->m_associationData.swap(newAssociationMap);
}

void QTOZW_Associations_internal::setAssociationData(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance, QTOZW_Associations::associationColumns column, QVariant data)
{
    int row = this->getassocationRow(_nodeID, _groupIDX, _targetNodeId, _targetNodeInstance);
    if (row == -1) {
        qWarning() << "setAssocationData: Group " << _groupIDX << "for node " << _nodeID << " does not exist";
        return;
    }
    if (this->m_associationData[row][column] != data) {
        this->m_associationData[row][column] = data;
        QVector<int> roles;
        roles << Qt::DisplayRole;
        this->dataChanged(this->createIndex(row, column), this->createIndex(row, column), roles);
    }
}

void QTOZW_Associations_internal::setAssociationFlags(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance, QTOZW_Associations::associationFlags _flags, bool _value)
{
    int row = this->getassocationRow(_nodeID, _groupIDX, _targetNodeId, _targetNodeInstance);
    if (row == -1) {
        qWarning() << "setAssocationFlags: Group " << _groupIDX << " for node " << _nodeID << " does not exist";
        return;
    }
    QBitArray flag = this->m_associationData[row][associationColumns::Flags].toBitArray();
    if (flag.at(_flags) != _value) {
        flag.setBit(_flags, _value);
        this->m_associationData[row][associationColumns::Flags] = flag;
        QVector<int> roles;
        roles << Qt::DisplayRole;
        this->dataChanged(this->createIndex(row, associationColumns::Flags), this->createIndex(row, associationColumns::Flags), roles);
    }
}


void QTOZW_Associations_internal::delNode(quint8 _nodeID) {
    QMap<int32_t, QMap<associationColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<associationColumns, QVariant> > newAssociationMap;
    int32_t newrow = 0;
    for (it = this->m_associationData.begin(); it != this->m_associationData.end(); ++it) {
        if (it.value()[associationColumns::NodeID] == _nodeID) {
            qDebug() << "Removing Node " << it.value()[associationColumns::NodeID] << it.key();
            this->beginRemoveRows(QModelIndex(), it.key(), it.key());
            this->m_associationData.erase(it);
            this->endRemoveRows();
            continue;
        } else {
            newAssociationMap[newrow] = it.value();
            newrow++;
        }
    }
    this->m_associationData.swap(newAssociationMap);
}

void QTOZW_Associations_internal::delGroup(quint8 _nodeID, quint8 _groupIDX) {
    QMap<int32_t, QMap<associationColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<associationColumns, QVariant> > newAssociationMap;
    int32_t newrow = 0;
    for (it = this->m_associationData.begin(); it != this->m_associationData.end(); ++it) {
        if (it.value()[associationColumns::NodeID] == _nodeID) {
            if (it.value()[associationColumns::GroupID] == _groupIDX) {
                qDebug() << "Removing Node " << it.value()[associationColumns::NodeID] << it.key();
                this->beginRemoveRows(QModelIndex(), it.key(), it.key());
                this->m_associationData.erase(it);
                this->endRemoveRows();
                continue;
            }
        }
        newAssociationMap[newrow] = it.value();
        newrow++;
    }
    this->m_associationData.swap(newAssociationMap);
}


void QTOZW_Associations_internal::resetModel() {
    this->beginRemoveRows(QModelIndex(), 0, this->m_associationData.count());
    this->m_associationData.clear();
    this->endRemoveRows();
}


void QTOZW_Associations_internal::setDefaultGroupData(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationColumns column, QVariant data) {
    switch (column) {
        case NodeID:
        case GroupID:
        case MemberNodeID:
        case MemberNodeInstance:
        {
            qWarning() << "Can Not Set Default Data for " << static_cast<associationColumns>(column);
            return;
        }
        default:
            break;
    }

    if (this->m_defaultData.count(_nodeID) == 0) {
        QMap<quint8, QMap<QTOZW_Associations::associationColumns, QVariant> > newNode;
        this->m_defaultData.insert(_nodeID, newNode);
    }
    if (this->m_defaultData[_nodeID].count(_groupIDX) == 0) {
        QBitArray flags(static_cast<int>(QTOZW_Nodes::flagCount));
        this->m_defaultData[_nodeID][_groupIDX][associationColumns::Flags] = flags;
    }
    this->m_defaultData[_nodeID][_groupIDX][column] = data;
    QMap<int32_t, QMap<QTOZW_Associations::associationColumns, QVariant> >::iterator it;
    for (it = this->m_associationData.begin(); it != this->m_associationData.end(); ++it) {
        if (it.value()[associationColumns::NodeID] == _nodeID) {
            if (it.value()[associationColumns::GroupID] == _groupIDX) {
                this->setAssociationData(_nodeID, _groupIDX, it.value()[associationColumns::MemberNodeID].value<quint8>(), it.value()[associationColumns::MemberNodeInstance].value<quint8>(), column, data);
            }
        }
    }
}

void QTOZW_Associations_internal::setDefaultGroupFlags(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationFlags flag, bool data) {
    if (this->m_defaultData.count(_nodeID) == 0) {
        QMap<quint8, QMap<QTOZW_Associations::associationColumns, QVariant> > newNode;
        this->m_defaultData.insert(_nodeID, newNode);
    }
    if (this->m_defaultData[_nodeID].count(_groupIDX) == 0) {
        QBitArray flags(static_cast<int>(QTOZW_Nodes::flagCount));
        this->m_defaultData[_nodeID][_groupIDX][associationColumns::Flags] = flags;
    }
    QBitArray flags = this->m_defaultData[_nodeID][_groupIDX][associationColumns::Flags].toBitArray();
    flags.setBit(flag, data);
    this->m_defaultData[_nodeID][_groupIDX][associationColumns::Flags].toBitArray() = flags;
    QMap<int32_t, QMap<QTOZW_Associations::associationColumns, QVariant> >::iterator it;
    for (it = this->m_associationData.begin(); it != this->m_associationData.end(); ++it) {
        if (it.value()[associationColumns::NodeID] == _nodeID) {
            if (it.value()[associationColumns::GroupID] == _groupIDX) {
                this->setAssociationFlags(_nodeID, _groupIDX, it.value()[associationColumns::MemberNodeID].value<quint8>(), it.value()[associationColumns::MemberNodeInstance].value<quint8>(), flag, data);
            }
        }
    }
}

bool QTOZW_Associations_internal::isAssociationExists(quint8 _node, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance) {
    return this->getassocationRow(_node, _groupIDX, _targetNodeId, _targetNodeInstance) == -1 ? false : true;
}
