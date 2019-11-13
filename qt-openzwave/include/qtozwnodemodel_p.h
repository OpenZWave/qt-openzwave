//-----------------------------------------------------------------------------
//
//	qtozwnodemodel_p.h
//
//	Node Model - Internal Class to Manage the Model
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

#ifndef QTOZWNODEMODEL_P_H
#define QTOZWNODEMODEL_P_H

#include "qt-openzwave/qtopenzwave.h"

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
