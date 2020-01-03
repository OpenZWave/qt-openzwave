#ifndef MQTTASSOC_H
#define MQTTASSOC_H

#include <QObject>
#include <QVariant>
#include <rapidjson/document.h>
#include "qt-openzwave/qtozwassociationmodel.h"

class mqttAssociationModel : public QTOZW_Associations {
    Q_OBJECT
public:
    explicit mqttAssociationModel(QObject *parent = nullptr);
    bool populateJsonObject(rapidjson::Document &, quint8, quint8, QTOZWManager *);
};


#endif