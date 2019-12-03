
#include <QVariant>
#include <QDebug>
#include <QLoggingCategory>
#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h> // for stringify JSON
#include "qtrj.h"

Q_LOGGING_CATEGORY(ozwqt2js, "ozw.mqtt.qt2js");

bool QT2JS::SetString(rapidjson::Document &doc, QString field, QString value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kStringType);
        v.SetString(value.toStdString().c_str(), doc.GetAllocator());
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsString()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type String: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()].GetString() != value) {
            doc[field.toStdString().c_str()].SetString(value.toStdString().c_str(), doc.GetAllocator());
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value: " << value;
        }
    }
    return false;
}
bool QT2JS::SetInt(rapidjson::Document &doc, QString field, qint32 value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kNumberType);
        v.SetInt(value);
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsInt()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type INT: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()].GetInt() != value) {
            doc[field.toStdString().c_str()].SetInt(value);
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value:" << value;
        }
    }
    return false;
}
bool QT2JS::SetUint(rapidjson::Document &doc, QString field, quint32 value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kNumberType);
        v.SetUint(value);
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsUint()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type UINT: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()].GetUint() != value) {
            doc[field.toStdString().c_str()].SetUint(value);
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value: " << value;
        }
    }
    return false;
}
bool QT2JS::SetInt64(rapidjson::Document &doc, QString field, qint64 value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kNumberType);
        v.SetInt64(value);
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsInt64()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type INT64: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()].GetInt64() != value) {
            doc[field.toStdString().c_str()].SetInt64(value);
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value: " << value;
        }
    }
    return false;
}
bool QT2JS::SetUInt64(rapidjson::Document &doc, QString field, quint64 value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kNumberType);
        v.SetUint64(value);
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsUint64()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type UINT64: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()].GetUint() != value) {
            doc[field.toStdString().c_str()].SetUint64(value);
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value: " << value;
        }
    }
    return false;
}
bool QT2JS::SetBool(rapidjson::Document &doc, QString field, bool value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kFalseType);
        v.SetBool(value);
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsBool()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type Bool: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()].GetBool() != value) {
            doc[field.toStdString().c_str()].SetBool(value);
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value: " << value;
        }
    }
    return false;
}
bool QT2JS::SetDouble(rapidjson::Document &doc, QString field, double value) {
    if (!doc.IsObject())
        doc.SetObject();
    if (!doc.HasMember(field.toStdString().c_str())) {
        rapidjson::Value v(rapidjson::kNumberType);
        v.SetDouble(value);
        doc.AddMember(rapidjson::Value(field.toStdString().c_str(), doc.GetAllocator()).Move(), v.Move(), doc.GetAllocator());
        return true;
    } else {
        if (!doc[field.toStdString().c_str()].IsDouble()) {
            qCWarning(ozwqt2js) << "Field " << field << " Is Not of Type Double: " << doc[field.toStdString().c_str()].GetType();
            return false;
        } else if (doc[field.toStdString().c_str()] != value) {
            doc[field.toStdString().c_str()].SetDouble(value);
            return true;
        } else {
            qCDebug(ozwqt2js) << "Field is Unchanged: " << field << " Value: " << value;
        }
    }
    return false;
}

QByteArray QT2JS::getJSON(rapidjson::Document &doc) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    return sb.GetString();
}

bool QT2JS::removeField(rapidjson::Document &doc, QString field) {
    if (!doc.IsObject())
        doc.SetObject();
    if (doc.HasMember(field.toStdString().c_str())) {
        return doc.RemoveMember(field.toStdString().c_str());
    } else {
        return false;
    }
}
