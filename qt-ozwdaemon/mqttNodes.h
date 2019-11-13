#ifndef MQTTNODES_H
#define MQTTNODES_H

#include <QObject>
#include <QVariant>
#include <rapidjson/document.h>
#include "qt-openzwave/qtozwnodemodel.h"

class mqttNodeModel : public QTOZW_Nodes {
    Q_OBJECT
public:
    explicit mqttNodeModel(QObject *parent = nullptr);
    QVariant getNodeData(quint8, NodeColumns);
    bool populateJsonObject(rapidjson::Document &, quint8, QTOZWManager *);
    bool isValidNode(quint8);
};


#endif