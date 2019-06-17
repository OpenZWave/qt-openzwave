#ifndef QTOZWVALUEIDMODEL_H
#define QTOZWVALUEIDMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QBitArray>


struct QTOZW_ValueIDList {
    QList< quint32 > values;
    QStringList labels;
    QString selectedItem;
};
QDataStream & operator<<( QDataStream & dataStream, const QTOZW_ValueIDList & list );
QDataStream & operator>>(QDataStream & dataStream, QTOZW_ValueIDList & list);

Q_DECLARE_METATYPE(QTOZW_ValueIDList);

struct QTOZW_ValueIDBitSet {
    QBitArray values;
    QBitArray mask;
    QMap<quint32, QString> label;
    QMap<quint32, QString> help;
};
QDataStream & operator<<( QDataStream & dataStream, const QTOZW_ValueIDBitSet & list );
QDataStream & operator>>(QDataStream & dataStream, QTOZW_ValueIDBitSet & list);

Q_DECLARE_METATYPE(QTOZW_ValueIDBitSet);


class QTOZW_ValueIds : public QAbstractTableModel {
    Q_OBJECT
public:
    enum ValueIdColumns {
        Label,
        Value,
        Units,
        Min,
        Max,
        Type,
        Instance,
        CommandClass,
        Index,
        Node,
        Genre,
        Help,
        ValueIDKey,
        ValueFlags,
        ValueIdCount
    };
    Q_ENUM(ValueIdColumns)
    enum ValueIdGenres {
        Basic,
        User,
        Config,
        System,
        GenreCount
    };
    Q_ENUM(ValueIdGenres)
    enum ValueIdTypes {
        Bool,
        Byte,
        Decimal,
        Int,
        List,
        Schedule,
        Short,
        String,
        Button,
        Raw,
        BitSet,
        TypeCount
    };
    Q_ENUM(ValueIdTypes)
    enum ValueIDFlags {
        ReadOnly,
        WriteOnly,
        ValueSet,
        ValuePolled,
        ChangeVerified,
        FlagCount
    };
    Q_ENUM(ValueIDFlags)
    QTOZW_ValueIds(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;
protected:
    QVariant getValueData(quint64, ValueIdColumns);
    int32_t getValueRow(quint64 _node);

    QMap<int32_t, QMap<ValueIdColumns, QVariant> > m_valueData;
};

#endif // QTOZWVALUEIDMODEL_H
