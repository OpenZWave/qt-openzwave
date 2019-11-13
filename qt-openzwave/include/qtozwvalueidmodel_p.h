//-----------------------------------------------------------------------------
//
//	qtozwvalueidmodel_p.h
//
//	ValueID Model - Internal Class to Manage the Model
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

#ifndef QTOZWVALUEIDMODEL_P_H
#define QTOZWVALUEIDMODEL_P_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include "qt-openzwave/qtozwvalueidmodel.h"

class QTOZW_ValueIds_internal : public QTOZW_ValueIds {
    Q_OBJECT
public:
    QTOZW_ValueIds_internal(QObject *parent=nullptr);
public Q_SLOTS:
    void addValue(quint64 _vidKey);
    void setValueData(quint64 _vidKey, QTOZW_ValueIds::ValueIdColumns column, QVariant data, bool transaction);
    void setValueFlags(quint64 _vidKey, QTOZW_ValueIds::ValueIDFlags _flags, bool _value, bool transaction);
    void delValue(quint64 _vidKey);
    void delNodeValues(quint8 _node);
    void resetModel();
    void finishTransaction(quint64 _vidKey);
    QVariant getValueData(quint64, ValueIdColumns);

};


QString BitSettoQString(QBitArray ba);
quint32 BitSettoInteger(QBitArray ba);


#endif // QTOZWVALUEIDMODEL_P_H
