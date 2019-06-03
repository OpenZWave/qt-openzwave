#include <QBitArray>
#include "qtozw_logging.h"
#include "qtozwnodemodel_p.h"


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
