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
        isMultiInstance,
        FlagCount
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
#endif // QTOZWASSOCIATIONS_H
