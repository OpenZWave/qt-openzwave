#ifndef QTOZWVALUEIDMODEL_P_H
#define QTOZWVALUEIDMODEL_P_H

#include <QObject>
#include "qt-openzwave/qtozwvalueidmodel.h"

class QTOZW_ValueIds_internal : public QTOZW_ValueIds {
    Q_OBJECT
public:
    QTOZW_ValueIds_internal(QObject *parent=nullptr);
public Q_SLOTS:
    void addValue(quint64 _vidKey);
    void setValueData(quint64 _vidKey, QTOZW_ValueIds::ValueIdColumns column, QVariant data);
    void setValueFlags(quint64 _vidKey, QTOZW_ValueIds::ValueIDFlags _flags, bool _value);
    void delValue(quint64 _vidKey);
    void delNodeValues(quint8 _node);
    void resetModel();
};


QString BitSettoQString(QBitArray ba);
quint32 BitSettoInteger(QBitArray ba);


#endif // QTOZWVALUEIDMODEL_P_H
