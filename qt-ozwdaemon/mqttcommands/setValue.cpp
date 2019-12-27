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

    quint64 vidKey = msg["ValueIDKey"].GetUint();
    QBitArray flags = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::ValueFlags).value<QBitArray>();
    if (flags[QTOZW_ValueIds::ValueIDFlags::ReadOnly] == true) {
        return this->sendSimpleStatus(false, "ValueID is Read Only");
    }
    QTOZW_ValueIds::ValueIdTypes types = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type).value<QTOZW_ValueIds::ValueIdTypes>();
    QVariant data;
    switch (types) {
        case QTOZW_ValueIds::ValueIdTypes::BitSet: {
            qCWarning(ozwmcsv) << "BitSet Not Done Yet";
            return false;
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
        case QTOZW_ValueIds::ValueIdTypes::List: 
        case QTOZW_ValueIds::ValueIdTypes::Raw:
        case QTOZW_ValueIds::ValueIdTypes::Schedule: {
            qCWarning(ozwmcsv) << "List/Raw/Schedule Not Done Yet";
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