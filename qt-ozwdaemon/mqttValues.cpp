#include "qt-openzwave/qtozwmanager.h"
#include "mqttValues.h"
#include "qtrj.h"


Q_LOGGING_CATEGORY(ozwmpvalue, "ozw.mqtt.publisher.value");

mqttValueIDModel::mqttValueIDModel(QObject *parent) :
    QTOZW_ValueIds(parent)
{

}
QVariant mqttValueIDModel::getValueData(quint64 vidKey, ValueIdColumns col) {
    int row = this->getValueRow(vidKey);
    return this->data(this->index(row, col), Qt::DisplayRole);
}

bool mqttValueIDModel::isValidValueID(quint64 vidKey) {
    if (this->getValueRow(vidKey) == -1) 
        return false;
    return true;
}


bool mqttValueIDModel::populateJsonObject(rapidjson::Document &jsonobject, quint64 vidKey, QTOZWManager *mgr) {
    for (int i = 0; i < ValueIdColumns::ValueIdCount; i++) {
        QVariant data = this->getValueData(vidKey, static_cast<ValueIdColumns>(i));
        switch (static_cast<ValueIdColumns>(i)) {
        case ValueFlags: {
            QBitArray flag = data.toBitArray();
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIDFlags>();
            for (int j = 0; j < ValueIDFlags::FlagCount; j++) {
                QT2JS::SetBool(jsonobject, metaEnum.valueToKey(j), flag.at(j));
            }
            break;
        }
        case Value: {
            this->encodeValue(jsonobject, vidKey);
            break;
        }
        case Genre: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdGenres>();
            QT2JS::SetString(jsonobject, "Genre", metaEnum.valueToKey(data.toInt()));
            break;
        }
        case Type: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdTypes>();
            QT2JS::SetString(jsonobject, "Type", metaEnum.valueToKey(data.toInt()));
            break;
        }
        case CommandClass: {
            QT2JS::SetString(jsonobject, "CommandClass", mgr->getCommandClassString(data.toInt()));
            break;
        }

        default: {
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIdColumns>();
            if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QString) {
                QT2JS::SetString(jsonobject, metaEnum.valueToKey(i), data.toString());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Bool) {
                QT2JS::SetBool(jsonobject, metaEnum.valueToKey(i), data.toBool());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Int) {
                QT2JS::SetInt(jsonobject, metaEnum.valueToKey(i), data.toInt());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::UInt) {
                QT2JS::SetUint(jsonobject, metaEnum.valueToKey(i), data.toUInt());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::Float) {
                QT2JS::SetDouble(jsonobject, metaEnum.valueToKey(i), data.toDouble());
            } else if (static_cast<QMetaType::Type>(data.type()) == QMetaType::ULongLong) {
                QT2JS::SetUInt64(jsonobject, metaEnum.valueToKey(i), static_cast<qint64>(data.toULongLong()));
            } else {
                qCWarning(ozwmpvalue) << "mqttValueIDModel::populateJsonObject: Can't Convert " << data.type() << "(" << metaEnum.valueToKey(i) << ") to store in JsonObject: " << vidKey;
            }
            break;
        }

        }
    }
    return true;
}

bool mqttValueIDModel::encodeValue(rapidjson::Document &value, quint64 vidKey) {
    QVariant data = this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Value);
    QTOZW_ValueIds::ValueIdTypes type = this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Type).value<QTOZW_ValueIds::ValueIdTypes>();
    bool changed = false;
    switch (type) {
        case QTOZW_ValueIds::ValueIdTypes::BitSet: {
            if (!value.HasMember("Value")) { 
                rapidjson::Value bitsets(rapidjson::kArrayType);
                bitsets.SetArray();
                QTOZW_ValueIDBitSet vidbs = data.value<QTOZW_ValueIDBitSet>();
                int size = vidbs.mask.size();
                for (int i = 0; i < size; i++) {
                    if (vidbs.mask[i] == 1) {
                        rapidjson::Value bitset;
                        bitset.SetObject();
                        bitset.AddMember(
                            rapidjson::Value("Label", value.GetAllocator()).Move(),
                            rapidjson::Value(vidbs.label[i].toStdString().c_str(), value.GetAllocator()).Move(),
                            value.GetAllocator()
                        );
                        bitset.AddMember(
                            rapidjson::Value("Help", value.GetAllocator()).Move(), 
                            rapidjson::Value(vidbs.help[i].toStdString().c_str(), value.GetAllocator()).Move(),
                            value.GetAllocator()
                        );
                        bitset.AddMember(
                            rapidjson::Value("Value", value.GetAllocator()).Move(),
                            rapidjson::Value(vidbs.values[i]),
                            value.GetAllocator()
                        );
                        bitset.AddMember(
                            rapidjson::Value("Position", value.GetAllocator()).Move(),
                            rapidjson::Value(i),
                            value.GetAllocator()
                        );
                        bitsets.PushBack(bitset, value.GetAllocator());
                    }
                }
                value.AddMember(
                    rapidjson::Value("Value", value.GetAllocator()).Move(),
                    bitsets,
                    value.GetAllocator()
                );
                changed = true;
            } else {
                /* Value Already Exists, Just Check for Changes */
                rapidjson::Value bitsets = value["Value"].GetArray();
                QTOZW_ValueIDBitSet vidbs = data.value<QTOZW_ValueIDBitSet>();
                for (unsigned int i = 0; i < bitsets.Size(); i++) {
                    rapidjson::Value bitset = bitsets[i].GetObject();
                    if (bitset.HasMember("Position") && bitset.HasMember("Value") && bitset["Position"].IsInt() && bitset["Value"].IsBool()) { 
                        int pos = bitset["Position"].GetInt();
                        bool val = bitset["Value"].GetBool();
                        if (vidbs.values[pos] != val) {
                            bitset["Value"].SetBool(val);
                            changed = true;
                        }
                    } else {
                        qCWarning(ozwmpvalue) << "Bitset is Missing Position or Value Members, or Incorrect Types: " << QT2JS::getJSON(value);
                    }
                }
            }
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            changed = QT2JS::SetBool(value, "Value", data.toBool());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Button: {
            changed = QT2JS::SetBool(value, "Value", data.toBool());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            changed = QT2JS::SetInt(value, "Value", data.toInt());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Decimal: {
            changed = QT2JS::SetDouble(value, "Value", data.toDouble());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int:{
            changed = QT2JS::SetInt(value, "Value", data.toInt());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::List: {
            QTOZW_ValueIDList vidlist = data.value<QTOZW_ValueIDList>();
            int size = vidlist.values.count();
            rapidjson::Value list(rapidjson::kArrayType);
            list.SetArray();
            if (!value.HasMember("Value")) {
                for (int i = 0; i < size; i++) {
                    rapidjson::Value entry;
                    entry.SetObject();
                    entry.AddMember(
                        rapidjson::Value("Value", value.GetAllocator()).Move(),
                        vidlist.values[i],
                        value.GetAllocator()
                    );
                    entry.AddMember(
                        rapidjson::Value("Label", value.GetAllocator()).Move(),
                        rapidjson::Value(vidlist.labels[i].toStdString().c_str(), value.GetAllocator()),
                        value.GetAllocator()
                    );
                    list.PushBack(entry, value.GetAllocator());
                }
                rapidjson::Value var(rapidjson::kObjectType);
                var.SetObject();
                var.AddMember(
                    rapidjson::Value("List", value.GetAllocator()).Move(),
                    list,
                    value.GetAllocator()
                );
                var.AddMember(
                    rapidjson::Value("Selected", value.GetAllocator()).Move(),
                    rapidjson::Value(vidlist.selectedItem.toStdString().c_str(), value.GetAllocator()).Move(),
                    value.GetAllocator()
                );
                var.AddMember(
                    rapidjson::Value("Selected_id", value.GetAllocator()).Move(),
                    vidlist.selectedItemId,
                    value.GetAllocator()
                );
                value.AddMember(
                    rapidjson::Value("Value", value.GetAllocator()).Move(),
                    var,
                    value.GetAllocator()
                );
                changed = true;
            } else {
                /* Value Array exists, so just check/update the Values */
                QString selected = value["Value"]["Selected"].GetString();
                if (selected != vidlist.selectedItem) {
                    value["Value"]["Selected"].SetString(vidlist.selectedItem.toStdString().c_str(), value.GetAllocator());
                    changed = true;
                }
                if (value["Value"]["Selected_id"].GetUint() != vidlist.selectedItemId) {
                    value["Value"]["Selected_id"].SetUint(vidlist.selectedItemId);
                    changed = true;
                }
                if (!changed) {
                    qCDebug(ozwmpvalue) << "List Selected Value has Not Changed: " << vidlist.selectedItem;
                }
            }
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Raw: {
            QByteArray ba = data.value<QByteArray>();
            changed = QT2JS::SetString(value, "Value", ba.toHex());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Schedule: {
            qCWarning(ozwmpvalue) << "Schedule ValueType not handled in mqttValueIdModel::encodeValue yet";
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            changed = QT2JS::SetInt(value, "Value" ,data.toInt());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::String: {
            changed = QT2JS::SetString(value, "Value", data.toString());
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::TypeCount: {
            qCWarning(ozwmpvalue) << "Unhandled ValueID Type" << type << "in mqttValueIdModel::encodeValue" << vidKey << this->getValueData(vidKey, mqttValueIDModel::ValueIdColumns::Label).toString();
            break;
        }
    }
    { /* send some of our "dynamic" attributes (Units etc) */
        QVariant data = this->getValueData(vidKey, ValueIdColumns::Units);
        if (!data.isNull()) {
            changed |= QT2JS::SetString(value, "Units", data.toString());
        }
        data = this->getValueData(vidKey, ValueIdColumns::ValueFlags);
        if (!data.isNull()) {
            QBitArray flag = data.toBitArray();
            QMetaEnum metaEnum = QMetaEnum::fromType<ValueIDFlags>();
            changed |= QT2JS::SetBool(value, metaEnum.valueToKey(ValueIDFlags::ValueSet), flag.at(ValueIDFlags::ValueSet));
            changed |= QT2JS::SetBool(value, metaEnum.valueToKey(ValueIDFlags::ValuePolled), flag.at(ValueIDFlags::ValuePolled));
            changed |= QT2JS::SetBool(value, metaEnum.valueToKey(ValueIDFlags::ChangeVerified), flag.at(ValueIDFlags::ChangeVerified));
        }
    }
    return changed;
}

bool mqttValueIDModel::setData(quint64 vidKey, QVariant data) {
    return QTOZW_ValueIds::setData(this->index(this->getValueRow(vidKey), QTOZW_ValueIds::ValueIdColumns::Value), data);
}


