#ifndef MQTTVALUES_H
#define MQTTVALUES_H

#include <QObject>
#include <QVariant>
#include <rapidjson/document.h>
#include "qt-openzwave/qtozwvalueidmodel.h"


class mqttValueIDModel : public QTOZW_ValueIds {
    Q_OBJECT
public:
    using QTOZW_ValueIds::setData;
    explicit mqttValueIDModel(QObject *parent = nullptr);
    QVariant getValueData(quint64, ValueIdColumns);
    bool populateJsonObject(rapidjson::Document &, quint64, QTOZWManager *);
    bool encodeValue(rapidjson::Document &, quint64);
    bool isValidValueID(quint64);
    bool setData(quint64, QVariant);
};


#endif