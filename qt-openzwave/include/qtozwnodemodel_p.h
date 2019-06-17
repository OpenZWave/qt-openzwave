#ifndef QTOZWNODEMODEL_P_H
#define QTOZWNODEMODEL_P_H

#include <QObject>
#include "qt-openzwave/qtozwnodemodel.h"

class QTOZW_Nodes_internal : public QTOZW_Nodes {
    Q_OBJECT
public:
    QTOZW_Nodes_internal(QObject *parent=nullptr);
public Q_SLOTS:
    void addNode(quint8 _nodeID);
    void setNodeData(quint8 _nodeID, QTOZW_Nodes::NodeColumns column, QVariant data, bool transaction);
    void setNodeFlags(quint8 _nodeID, QTOZW_Nodes::nodeFlags _flags, bool _value, bool transaction);
    void delNode(quint8 _nodeID);
    void resetModel();
    void finishTransaction(quint8 _nodeID);
};


#endif // QTOZWNODEMODEL_P_H
