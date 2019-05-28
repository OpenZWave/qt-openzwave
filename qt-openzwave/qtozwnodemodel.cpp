#include <QDebug>
#include <QBitArray>
#include "qtozw_logging.h"
#include "qtozwnodemodel.h"
#include "qtopenzwave.h"


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
        QMap<NodeColumns, QVariant> node = this->m_nodeData[index.row()];
        if (node.size() == 0) {
            qCWarning(nodeModel) << "data: Cant find any Node on Row " << index.row();
            return QVariant();
        }
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
            if (this->m_nodeData[index.row()][static_cast<NodeColumns>(index.column())] != value) {
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
        return this->m_nodeData[row][_column];
    qCWarning(nodeModel) << "Can't find NodeData for Node " << _node;
    return QVariant();
}

int32_t QTOZW_Nodes::getNodeRow(quint8 _node) {
    if (this->m_nodeData.count() == 0) {
        return -1;
    }
    QMap<int32_t, QMap<NodeColumns, QVariant> >::iterator it;
    for (it = m_nodeData.begin(); it != m_nodeData.end(); ++it) {
        QMap<NodeColumns, QVariant> node = it.value();
        if (node.value(QTOZW_Nodes::NodeID) == _node) {
            return it.key();
        }
    }
    qCWarning(nodeModel) << "Can't Find NodeID " << _node << " in NodeData";
    return -1;
}




QTOZW_Nodes_internal::QTOZW_Nodes_internal(QObject *parent)
    : QTOZW_Nodes(parent)
{

}

void QTOZW_Nodes_internal::addNode(quint8 _nodeID)
{
    if (this->getNodeRow(_nodeID) >= 0) {
        qCWarning(nodeModel) << "Node " << _nodeID << " Already Exists";
        return;
    }
    QMap<QTOZW_Nodes::NodeColumns, QVariant> newNode;
    newNode[QTOZW_Nodes::NodeID] = _nodeID;
    QBitArray flags(static_cast<int>(QTOZW_Nodes::flagCount));
    newNode[QTOZW_Nodes::NodeFlags] = flags;

    this->beginInsertRows(QModelIndex(), this->rowCount(QModelIndex()), this->rowCount(QModelIndex()));
    this->m_nodeData[this->rowCount(QModelIndex())] = newNode;
    this->endInsertRows();
}

void QTOZW_Nodes_internal::setNodeData(quint8 _nodeID, QTOZW_Nodes::NodeColumns column, QVariant data)
{
    int row = this->getNodeRow(_nodeID);
    if (row == -1) {
        qCWarning(nodeModel) << "setNodeData: Node " << _nodeID << " does not exist";
        return;
    }
    if (this->m_nodeData[row][column] != data) {
        this->m_nodeData[row][column] = data;
        QVector<int> roles;
        roles << Qt::DisplayRole;
        this->dataChanged(this->createIndex(row, column), this->createIndex(row, column), roles);
    }
}

void QTOZW_Nodes_internal::setNodeFlags(quint8 _nodeID, QTOZW_Nodes::nodeFlags _flags, bool _value)
{
    int row = this->getNodeRow(_nodeID);
    if (row == -1) {
        qCWarning(nodeModel) << "setNodeData: Node " << _nodeID << " does not exist";
        return;
    }
    QBitArray flag = this->m_nodeData[row][QTOZW_Nodes::NodeFlags].toBitArray();
    if (flag.at(_flags) != _value) {
        flag.setBit(_flags, _value);
        this->m_nodeData[row][QTOZW_Nodes::NodeFlags] = flag;
        QVector<int> roles;
        roles << Qt::DisplayRole;
        this->dataChanged(this->createIndex(row, QTOZW_Nodes::NodeFlags), this->createIndex(row, QTOZW_Nodes::NodeFlags), roles);
    }
}
void QTOZW_Nodes_internal::delNode(quint8 _nodeID) {
    QMap<int32_t, QMap<NodeColumns, QVariant> >::iterator it;
    QMap<int32_t, QMap<NodeColumns, QVariant> > newNodeMap;
    int32_t newrow = 0;
    for (it = this->m_nodeData.begin(); it != this->m_nodeData.end(); ++it) {
        if (it.value()[QTOZW_Nodes::NodeColumns::NodeID] == _nodeID) {
            qCDebug(nodeModel) << "Removing Node " << it.value()[QTOZW_Nodes::NodeColumns::NodeID] << it.key();
            this->beginRemoveRows(QModelIndex(), it.key(), it.key());
            this->m_nodeData.erase(it);
            this->endRemoveRows();
            continue;
        } else {
            newNodeMap[newrow] = it.value();
            newrow++;
        }
    }
    this->m_nodeData.swap(newNodeMap);
}

void QTOZW_Nodes_internal::resetModel() {
    this->beginRemoveRows(QModelIndex(), 0, this->m_nodeData.count());
    this->m_nodeData.clear();
    this->endRemoveRows();
}
