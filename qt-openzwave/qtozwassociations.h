#ifndef QTOZWASSOCIATIONS_H
#define QTOZWASSOCIATIONS_H

#include <QObject>
#include <QAbstractItemModel>

class QTOZW_Associations : public QAbstractTableModel {
    Q_OBJECT
public:
    enum associationColumns {
        NodeID,
        GroupID,
        GroupName,
        GroupHelp,
        MaxAssocations,
        MemberNodeID,
        MemberNodeInstance,
        Flags,
        Count
    };
    Q_ENUM(associationColumns)

    enum associationFlags {
        isMultiInstance
    };

    Q_ENUM(associationFlags)

    friend class QTOZWManager_Internal;

    QTOZW_Associations(QObject *parent=nullptr);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
    QVariant getassocationData(quint8 _node, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance, QTOZW_Associations::associationColumns _column);
    int32_t getassocationRow(quint8 _node, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance);

    QMap<int32_t, QMap<QTOZW_Associations::associationColumns, QVariant> > m_associationData;
};

class QTOZW_Associations_internal : public QTOZW_Associations {
    Q_OBJECT
public:
    QTOZW_Associations_internal(QObject *parent=nullptr);
public Q_SLOTS:
    void addAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance);
    void delAssociation(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance);
    void setAssociationData(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance, QTOZW_Associations::associationColumns column, QVariant data);
    void setAssociationFlags(quint8 _nodeID, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance, QTOZW_Associations::associationFlags _flags, bool _value);
    bool isAssociationExists(quint8 _node, quint8 _groupIDX, quint8 _targetNodeId, quint8 _targetNodeInstance);
    void setDefaultGroupData(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationColumns column, QVariant data);
    void setDefaultGroupFlags(quint8 _nodeID, quint8 _groupIDX, QTOZW_Associations::associationFlags _flags, bool _value);
    void delNode(quint8 _nodeID);
    void delGroup(quint8 _nodeID, quint8 _groupIDX);
    void resetModel();
private:

    QMap<quint8, QMap<quint8, QMap<QTOZW_Associations::associationColumns, QVariant> > > m_defaultData;
};

#endif // QTOZWASSOCIATIONS_H
