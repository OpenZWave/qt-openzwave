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
        Members,
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
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

protected:
    QVariant getassocationData(quint8 _node, quint8 _groupIDX, QTOZW_Associations::associationColumns _column);
    int32_t getassocationRow(quint8 _node, quint8 _groupIDX);

    QMap<qint32, QMap<QTOZW_Associations::associationColumns, QVariant> > m_associationData;
};

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

#endif // QTOZWASSOCIATIONS_H
