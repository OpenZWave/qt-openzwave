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
    QVariant getassocationData(uint8_t _node, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance, QTOZW_Associations::associationColumns _column);
    int32_t getassocationRow(uint8_t _node, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance);

    QMap<int32_t, QMap<QTOZW_Associations::associationColumns, QVariant> > m_associationData;
};

class QTOZW_Associations_internal : public QTOZW_Associations {
    Q_OBJECT
public:
    QTOZW_Associations_internal(QObject *parent=nullptr);
public Q_SLOTS:
    void addAssociation(uint8_t _nodeID, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance);
    void delAssociation(uint8_t _nodeID, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance);
    void setAssociationData(uint8_t _nodeID, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance, QTOZW_Associations::associationColumns column, QVariant data);
    void setAssociationFlags(uint8_t _nodeID, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance, QTOZW_Associations::associationFlags _flags, bool _value);
    bool isAssociationExists(uint8_t _node, uint8_t _groupIDX, uint8_t _targetNodeId, uint8_t _targetNodeInstance);
    void setDefaultGroupData(uint8_t _nodeID, uint8_t _groupIDX, QTOZW_Associations::associationColumns column, QVariant data);
    void setDefaultGroupFlags(uint8_t _nodeID, uint8_t _groupIDX, QTOZW_Associations::associationFlags _flags, bool _value);
    void delNode(uint8_t _nodeID);
    void delGroup(uint8_t _nodeID, uint8_t _groupIDX);
    void resetModel();
private:

    QMap<uint8_t, QMap<uint8_t, QMap<QTOZW_Associations::associationColumns, QVariant> > > m_defaultData;
};

#endif // QTOZWASSOCIATIONS_H
