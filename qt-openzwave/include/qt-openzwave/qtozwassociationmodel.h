//-----------------------------------------------------------------------------
//
//	qtozwassociationmodel.h
//
//	Association Groups Exposed as a ABstractTableModel
//
//	Copyright (c) 2019 Justin Hammond <Justin@dynam.ac>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of QT-OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#ifndef QTOZWASSOCIATIONS_H
#define QTOZWASSOCIATIONS_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QAbstractItemModel>


class QTOPENZWAVESHARED_EXPORT QTOZW_Associations : public QAbstractTableModel {
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
#if 0
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
#endif

protected:
    QVariant getassocationData(quint8 _node, quint8 _groupIDX, QTOZW_Associations::associationColumns _column);
    int32_t getassocationRow(quint8 _node, quint8 _groupIDX);

    QMap<qint32, QMap<QTOZW_Associations::associationColumns, QVariant> > m_associationData;
};
#endif // QTOZWASSOCIATIONS_H
