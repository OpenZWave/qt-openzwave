
#include "qt-openzwave/qtozwmanager.h"
#include "mqttNodes.h"
#include "qtrj.h"


Q_LOGGING_CATEGORY(ozwmpnode, "ozw.mqtt.publisher.node");

mqttNodeModel::mqttNodeModel(QObject *parent) :
    QTOZW_Nodes(parent)
{

}

QVariant mqttNodeModel::getNodeData(quint8 node, NodeColumns col) {
    int row = this->getNodeRow(node);
    return this->data(this->index(row, col), Qt::DisplayRole);
}

bool mqttNodeModel::isValidNode(quint8 node) {
    if (this->getNodeRow(node) == -1)
        return false;
    return true;
}


bool mqttNodeModel::populateJsonObject(rapidjson::Document &jsonobject, quint8 node, QTOZWManager *mgr) {
    if (jsonobject.IsNull())
        jsonobject.SetObject();

    for (int i = 0; i < this->columnCount(QModelIndex()); i++) {
        QVariant data = this->getNodeData(node, static_cast<NodeColumns>(i));
        if (data.type() == QVariant::Invalid) {
            continue;
        }
        switch (static_cast<NodeColumns>(i)) {
            case NodeColumns::NodeFlags: {
                QBitArray flag = data.toBitArray();
                QMetaEnum metaEnum = QMetaEnum::fromType<nodeFlags>();
                for (int j = 0; j < nodeFlags::flagCount; j++) {
                    QT2JS::SetBool(jsonobject, metaEnum.valueToKey(j), flag.at(j));
                }
                break;
            }
            default: {
                QMetaEnum metaEnum = QMetaEnum::fromType<NodeColumns>();
                if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QString) {
                    QT2JS::SetString(jsonobject, metaEnum.valueToKey(i), data.toString());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Bool) {
                    QT2JS::SetBool(jsonobject, metaEnum.valueToKey(i), data.toBool());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Int) {
                    QT2JS::SetInt(jsonobject, metaEnum.valueToKey(i), data.toInt());
                } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::UInt) {
                    QT2JS::SetUint(jsonobject, metaEnum.valueToKey(i), data.toUInt());
                } else {
                    qCWarning(ozwmpnode) << "Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << node;
                }
                break;
            }
        }

    }
    /* MetaData */
    rapidjson::Value metadata;
    if (!jsonobject.HasMember("MetaData")) {
        rapidjson::Value metadata;
        metadata.SetObject();
        QMetaEnum metaEnum = QMetaEnum::fromType<QTOZWManagerSource::QTOZWMetaDataField>();
        if (metadata.IsNull()) {
            metadata.SetObject();
        }
        for (int i = 0; i < QTOZWManagerSource::Identifier; i++) {
            metadata.AddMember(
                rapidjson::Value(metaEnum.valueToKey(i), jsonobject.GetAllocator()).Move(),
                rapidjson::Value(mgr->GetMetaData(node, static_cast<QTOZWManagerSource::QTOZWMetaDataField>(i)).toStdString().c_str(), jsonobject.GetAllocator()).Move(),
                jsonobject.GetAllocator());
        }
        metadata.AddMember(rapidjson::Value("ProductPicBase64").Move(), 
            rapidjson::Value(QString(mgr->GetMetaDataProductPic(node).toBase64()).toStdString().c_str(), jsonobject.GetAllocator()).Move(),
            jsonobject.GetAllocator());

        jsonobject.AddMember(rapidjson::Value("MetaData"), metadata, jsonobject.GetAllocator());
    }
    /* Neighbors */
    QVector<quint8> neighbors = mgr->GetNodeNeighbors(node);
    if (neighbors.size() > 0) {
        rapidjson::Value N(rapidjson::kArrayType);
        for (int i = 0; i < neighbors.count(); i++) {
            N.PushBack(neighbors[i], jsonobject.GetAllocator());
        }
        jsonobject.AddMember(rapidjson::Value("Neighbors").Move(), N, jsonobject.GetAllocator());
    }
    return true;
}
