//-----------------------------------------------------------------------------
//
//	qtozwassociationmodel_p.h
//
//	Association Groups Model - Used Internally to Manage the Model
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

#ifndef QTOZWASSOCIATIONMODEL_P_H
#define QTOZWASSOCIATIONMODEL_P_H

#include "qt-openzwave_global.h"
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
