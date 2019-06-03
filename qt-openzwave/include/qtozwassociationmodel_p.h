#ifndef QTOZWASSOCIATIONMODEL_P_H
#define QTOZWASSOCIATIONMODEL_P_H

#include <QObject>
#include "qt-openzwave/qtozwassociationmodel.h"

class QTOZW_Associations_internal : public QTOZW_Associations {
    Q_OBJECT
public:
    QTOZW_Associations_internal(QObject *parent=nullptr);
public Q_SLOTS:
    void addGroup(quint8 _nodeID, quint8 _groupIDX);
    void setGroupData(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationColumns column, QVariant data);
    void setGroupFlags(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationFlags _flags, bool _value);
    void delNode(quint8 _nodeID);
    void addAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNode, quint8 _targetInstance);
    void delAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNode, quint8 _targetInstance);
    bool findAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNode, quint8 _targetInstance);
    void resetModel();
};


#endif // QTOZWASSOCIATIONMODEL_P_H
