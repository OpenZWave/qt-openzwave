#ifndef QTRJ_H
#define QTRJ_H

#include <QObject>
#include <QByteArray>
#include <rapidjson/document.h>

class QT2JS {
    public:
    static bool SetString(rapidjson::Document &, QString, QString);
    static bool SetInt(rapidjson::Document &, QString, qint32);
    static bool SetUint(rapidjson::Document &, QString, quint32);
    static bool SetInt64(rapidjson::Document &, QString, qint64);
    static bool SetUInt64(rapidjson::Document &, QString, quint64);
    static bool SetBool(rapidjson::Document &, QString, bool);
    static bool SetDouble(rapidjson::Document &, QString, double);
    static QByteArray getJSON(rapidjson::Document &);
    static bool removeField(rapidjson::Document &, QString);
};


#endif // QTRJ_H