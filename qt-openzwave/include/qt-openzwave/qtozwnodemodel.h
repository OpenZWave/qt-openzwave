//-----------------------------------------------------------------------------
//
//	qtozwnodemodel.h
//
//	Nodes Exposed as a QAbstractTableModel
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

#ifndef QTOZWNODEMODEL_H
#define QTOZWNODEMODEL_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include <QAbstractItemModel>

class QTOPENZWAVESHARED_EXPORT QTOZW_Nodes : public QAbstractTableModel {
    Q_OBJECT
public:
    enum NodeColumns {
        NodeID,
        NodeName,
        NodeLocation,
        NodeManufacturerName,
        NodeProductName,
        NodeBasicString,
        NodeBasic,
        NodeGenericString,
        NodeGeneric,
        NodeSpecificString,
        NodeSpecific,
        NodeDeviceTypeString,
        NodeDeviceType,
        NodeRole,
        NodeRoleString,
        NodePlusType,
        NodePlusTypeString,
        NodeQueryStage,
        NodeManufacturerID,
        NodeProductType,
        NodeProductID,
        NodeBaudRate,
        NodeVersion,
        NodeGroups,
        NodeFlags,
        NodeCount
    };
    Q_ENUM(NodeColumns)

    enum nodeFlags {
        isListening,
        isFlirs,
        isBeaming,
        isRouting,
        isSecurityv1,
        isZWavePlus,
        isNIFRecieved,
        isAwake,
        isFailed,
        flagCount
    };

    Q_ENUM(nodeFlags)

    QTOZW_Nodes(QObject *parent=nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) override;

protected:
    QVariant getNodeData(quint8, NodeColumns);
    int32_t getNodeRow(quint8 _node);

    QMap<int32_t, QMap<NodeColumns, QVariant> > m_nodeData;
};
#endif // QTOZWNODEMODEL_H
