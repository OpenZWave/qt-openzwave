#include "mqttcommands/setValue.h"

Q_LOGGING_CATEGORY(ozwmcsv, "ozw.mqtt.commands.setValue");


MqttCommand_SetValue::MqttCommand_SetValue(QObject *parent) :
    MqttCommand(parent)
{
    this->m_requiredIntFields << "ValueIDKey";
}
MqttCommand* MqttCommand_SetValue::Create(QObject *parent) {
    return new MqttCommand_SetValue(parent);
}

bool MqttCommand_SetValue::processMessage(rapidjson::Document &msg) {
    if (!this->checkValue(msg, "ValueIDKey")) {
        return this->sendSimpleStatus(false, "Invalid ValueIDKey Number");
    }
    /* check that the Value Field exists */
    if (!msg.HasMember("Value")) {
        return this->sendSimpleStatus(false, "Missing Field Value");
    }

    quint64 vidKey = msg["ValueIDKey"].GetUint64();
    QBitArray flags = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::ValueFlags).value<QBitArray>();
    if (flags[QTOZW_ValueIds::ValueIDFlags::ReadOnly] == true) {
        return this->sendSimpleStatus(false, "ValueID is Read Only");
    }
    QTOZW_ValueIds::ValueIdTypes types = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type).value<QTOZW_ValueIds::ValueIdTypes>();
    QVariant data;
    switch (types) {
        case QTOZW_ValueIds::ValueIdTypes::BitSet: {
            if (!msg["Value"].IsArray()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Array: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Array) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            rapidjson::Value bitsets = msg["Value"].GetArray();
            QTOZW_ValueIDBitSet bits = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value).value<QTOZW_ValueIDBitSet>();
            for (rapidjson::SizeType i = 0; i < bitsets.Size(); i++) {
                qint32 pos = -1;
                if (bitsets[i].HasMember("Label")) {
                    QString label = bitsets[i]["Label"].GetString();
                    pos = bits.label.key(label, -1);
                } else if (bitsets[i].HasMember("Position")) {
                    pos = bitsets[i]["Position"].GetUint();
                } else {
                    this->sendSimpleStatus(false, QString("BitSet Array Does not have a Label or Position Value: "));
                    qCWarning(ozwmcsv) << "BitSet Array does not have a Label or Position Value:" << GetCommand() << ": Value: " << msg["Value"].GetString();
                    return false;
                }
                if (pos < 0) {
                    this->sendSimpleStatus(false, QString("BitSet Array Does not have a Valid Label or Position Value: "));
                    qCWarning(ozwmcsv) << "BitSet Array does not have a Valid Label or Position Value:" << GetCommand() << ": Value: " << msg["Value"].GetString();
                    return false;
                }
                if (bits.values[pos] != bitsets[i]["Value"].GetBool()) {
                    bits.values.setBit(pos, bitsets[i]["Value"].GetBool());
                }
            }
            data = QVariant::fromValue<QTOZW_ValueIDBitSet>(bits);
            break;
        }

        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            if (!msg["Value"].IsBool()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Bool: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Bool) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            data = msg["Value"].GetBool();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Button: {
            if (!msg["Value"].IsBool()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Bool: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Bool) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            data = msg["Value"].GetBool();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            if (!msg["Value"].IsUint()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Byte: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Byte) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            if (msg["Value"].GetUint() > UCHAR_MAX) {
                this->sendSimpleStatus(false, QString("Value is Larger than Byte Field: ").append(msg["Value"].GetUint()));
                qCWarning(ozwmcsv) << "Value is Larger than Byte Field for " << GetCommand() << ": Value: " << msg["Value"].GetUint();
                return false;
            }
            data = msg["Value"].GetUint();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Decimal: {
            if (!msg["Value"].IsDouble()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Decimal: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Decimal) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            data = msg["Value"].GetDouble();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int: {
            if (!msg["Value"].IsUint()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Integer: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Integer) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            if (static_cast<uint>(msg["Value"].GetUint()) > UINT_MAX) {
                this->sendSimpleStatus(false, QString("Value is Larger than Integer Field: ").append(msg["Value"].GetUint()));
                qCWarning(ozwmcsv) << "Value is Larger than Integer Field for " << GetCommand() << ": Value: " << msg["Value"].GetUint();
                return false;
            }
            data = msg["Value"].GetUint();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::List: {
            if (!msg["Value"].IsUint()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Integer: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Integer) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            QTOZW_ValueIDList list = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Value).value<QTOZW_ValueIDList>();
            int index = list.values.indexOf(msg["Value"].GetUint());
            if (index < 0) {
                this->sendSimpleStatus(false, QString("Selected List Value is not in Lists: ").append(msg["Value"].GetUint()));
                qCWarning(ozwmcsv) << "Selected List Value is Not In List for " << GetCommand() << ": Value: " << msg["Value"].GetUint() << "List:" << list.labels;
                return false;
            }
            list.selectedItem = list.labels[index];
            list.selectedItemId = index;
            data.setValue<QTOZW_ValueIDList>(list);
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Raw: {
            if (!msg["Value"].IsString()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not String: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (String) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            data = QByteArray::fromHex(msg["Value"].GetString());
            break;
        }

        case QTOZW_ValueIds::ValueIdTypes::Schedule: {
            qCWarning(ozwmcsv) << "Raw/Schedule Not Done Yet";
            return false;

        }
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            if (!msg["Value"].IsUint()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not Short: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (Short) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            if (msg["Value"].GetUint() > USHRT_MAX) {
                this->sendSimpleStatus(false, QString("Value is Larger than Short Field: ").append(msg["Value"].GetUint()));
                qCWarning(ozwmcsv) << "Value is Larger than Short Field for " << GetCommand() << ": Value: " << msg["Value"].GetUint();
                return false;
            }
            data = msg["Value"].GetUint();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::String: {
            if (!msg["Value"].IsString()) {
                this->sendSimpleStatus(false, QString("Incorrect Field Type for Value: Not String: ").append(msg["Value"].GetType()));
                qCWarning(ozwmcsv) << "Incorrect Field Type (String) for " << GetCommand() << ": Value: " << msg["Value"].GetType();
                return false;
            }
            data = msg["Value"].GetString();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::TypeCount: {
            qCWarning(ozwmcsv) << "Invalid ValueID Type " << types << "for setValue";
            return this->sendSimpleStatus(false, "Unknown ValueID Type");
            break;
        }
    }
    if (data.isNull()) {
        qCWarning(ozwmcsv) << "Data is undefined for setValue... Json Conversion Failed?";
        return this->sendSimpleStatus(false, "JSON Conversion Failed");
    }
    qCInfo(ozwmcsv) << "Setting " << vidKey << " to Value " << data;
    return this->sendSimpleStatus(this->setValue(vidKey, data));
}