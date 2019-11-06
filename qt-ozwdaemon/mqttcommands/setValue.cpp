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

bool MqttCommand_SetValue::processMessage(QJsonDocument msg) {
    if (!this->checkValue(msg, "ValueIDKey")) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "Invalid ValueIDKey Number";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    /* check that the Value Field exists */
    if (msg["Value"].isUndefined()) {
        QJsonObject js;
        js["Error"]  = QString("Missing Field Value");
        emit sendCommandUpdate(GetCommand(), js);
        qCWarning(ozwmcsv) << "Missing Field for " << GetCommand() << ": Value: " << msg.toJson();
        return false;
    }

    quint64 vidKey = msg["ValueIdKey"].toInt();
    QBitArray flags = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::ValueFlags).value<QBitArray>();
    if (flags[QTOZW_ValueIds::ValueIDFlags::ReadOnly] == true) {
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "ValueID is Read Only";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    QTOZW_ValueIds::ValueIdTypes types = this->getValueData(vidKey, QTOZW_ValueIds::ValueIdColumns::Type).value<QTOZW_ValueIds::ValueIdTypes>();
    QVariant data;
    switch (types) {
        case QTOZW_ValueIds::ValueIdTypes::BitSet: {
            qCWarning(ozwmcsv) << "BitSet Not Done Yet";
            return false;
        }

        case QTOZW_ValueIds::ValueIdTypes::Bool: {
            if (!msg["Value"].isBool()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not Bool: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (Bool) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            data = msg["Value"].toBool();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Button: {
            if (!msg["Value"].isBool()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not Bool: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (Bool) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            data = msg["Value"].toBool();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Byte: {
            if (!msg["Value"].isDouble()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not Byte: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (Byte) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            if (msg["Value"].toInt() > UCHAR_MAX) {
                QJsonObject js;
                js["Error"]  = QString("Value is Larger than Byte Field: ").append(msg["Value"].toInt());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Value is Larger than Byte Field for " << GetCommand() << ": Value: " << msg["Value"].toInt() << msg.toJson();
                return false;
            }
            data = msg["Value"].toInt();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Decimal: {
            if (!msg["Value"].isDouble()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not Decimal: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (Decimal) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            data = msg["Value"].toDouble();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::Int: {
            if (!msg["Value"].isDouble()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not Integer: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (Integer) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            if (static_cast<uint>(msg["Value"].toInt()) > UINT_MAX) {
                QJsonObject js;
                js["Error"]  = QString("Value is Larger than Integer Field: ").append(msg["Value"].toInt());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Value is Larger than Integer Field for " << GetCommand() << ": Value: " << msg["Value"].toInt() << msg.toJson();
                return false;
            }
            data = msg["Value"].toInt();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::List: 
        case QTOZW_ValueIds::ValueIdTypes::Raw:
        case QTOZW_ValueIds::ValueIdTypes::Schedule: {
            qCWarning(ozwmcsv) << "List/Raw/Schedule Not Done Yet";
            return false;

        }
        case QTOZW_ValueIds::ValueIdTypes::Short: {
            if (!msg["Value"].isDouble()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not Short: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (Short) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            if (msg["Value"].toInt() > USHRT_MAX) {
                QJsonObject js;
                js["Error"]  = QString("Value is Larger than Short Field: ").append(msg["Value"].toInt());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Value is Larger than Short Field for " << GetCommand() << ": Value: " << msg["Value"].toInt() << msg.toJson();
                return false;
            }
            data = msg["Value"].toInt();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::String: {
            if (!msg["Value"].isString()) {
                QJsonObject js;
                js["Error"]  = QString("Incorrect Field Type for Value: Not String: ").append(msg["Value"].type());
                emit sendCommandUpdate(GetCommand(), js);
                qCWarning(ozwmcsv) << "Incorrect Field Type (String) for " << GetCommand() << ": Value: " << msg["Value"].type() << msg.toJson();
                return false;
            }
            data = msg["Value"].toString();
            break;
        }
        case QTOZW_ValueIds::ValueIdTypes::TypeCount: {
            qCWarning(ozwmcsv) << "Invalid ValueID Type " << types << "for setValue" << msg.toJson();
            QJsonObject js;
            js["status"] = "failed";
            js["Error"] = "Unknown ValueID Type";
            emit sendCommandUpdate(GetCommand(), js);
            return false;
            break;
        }
    }
    if (data.isNull()) {
        qCWarning(ozwmcsv) << "Data is undefined for setValue... Json Conversion Failed?";
        QJsonObject js;
        js["status"] = "failed";
        js["Error"] = "JSON Conversion Failed";
        emit sendCommandUpdate(GetCommand(), js);
        return false;
    }
    if (this->setValue(vidKey, data)) {
        QJsonObject js;
        js["status"] = "ok";
        emit sendCommandUpdate(GetCommand(), js);
        return true;
    }
    QJsonObject js;
    js["status"] = "failed";
    emit sendCommandUpdate(GetCommand(), js);
    return false;
}