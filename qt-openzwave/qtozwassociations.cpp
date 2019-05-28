#include <QDebug>
#include <QBitArray>
#include "qtozw_logging.h"
#include "qtozwassociations.h"
#include "qtopenzwave.h"


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
    QBitArray flags(static_cast<int>(QTOZW_Nodes::flagCount));
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
    QBitArray flags(static_cast<int>(QTOZW_Nodes::flagCount));
    flags = this->m_associationData[row][associationColumns::Flags].toBitArray();
    flags.setBit(_flags, _value);
    this->m_associationData[row][associationColumns::Flags] = flags;
    QVector<int> roles;
    roles << Qt::DisplayRole;
    this->dataChanged(this->createIndex(row, associationColumns::Flags), this->createIndex(row, associationColumns::Flags), roles);

};

void QTOZW_Associations_internal::delNode(quint8 _nodeID) {
    for (int i = 0; i <= rowCount(QModelIndex()); i++) {
        if (this->m_associationData[i][associationColumns::NodeID] == _nodeID) {
            qCDebug(associationModel) << "Removing Node " << this->m_associationData[i][associationColumns::NodeID] << i;
            this->beginRemoveRows(QModelIndex(), i, i);
            this->m_associationData.remove(i);
            for (int j = i+1; i <= rowCount(QModelIndex()); j++) {
                this->m_associationData[i] = this->m_associationData[j];
            }
            this->m_associationData.remove(rowCount(QModelIndex()));
            this->endRemoveRows();
            continue;
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
    target.append(QString::number(_targetNode)).append(":").append(QString::number(_targetInstance));
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
    target.append(QString::number(_targetNode)).append(":").append(QString::number(_targetInstance));
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
    target.append(QString::number(_targetNode)).append(":").append(QString::number(_targetInstance));
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
