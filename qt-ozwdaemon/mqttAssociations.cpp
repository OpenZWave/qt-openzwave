#include "qt-openzwave/qtozwmanager.h"
#include "mqttAssociations.h"
#include "qtrj.h"


Q_LOGGING_CATEGORY(ozwmpassociation, "ozw.mqtt.publisher.association");

mqttAssociationModel::mqttAssociationModel(QObject *parent) :
    QTOZW_Associations(parent)
{

}


bool mqttAssociationModel::populateJsonObject(rapidjson::Document &jsonobject, quint8 node, quint8 group, QTOZWManager *mgr) {
    Q_UNUSED(mgr);
    if (jsonobject.IsNull())
        jsonobject.SetObject();

    QT2JS::SetString(jsonobject, "Name", getassocationData(node, group, QTOZW_Associations::associationColumns::GroupName).toString());
    QT2JS::SetString(jsonobject, "Help", getassocationData(node, group, QTOZW_Associations::associationColumns::GroupHelp).toString());
    QT2JS::SetUint(jsonobject, "MaxAssociations", getassocationData(node, group, QTOZW_Associations::associationColumns::MaxAssocations).toInt());
    QStringList members = getassocationData(node, group, QTOZW_Associations::associationColumns::Members).toStringList();
    rapidjson::Value N(rapidjson::kArrayType);
    for (int i = 0; i < members.size(); i++) {
        rapidjson::Value strVal;
        strVal.SetString(members.at(i).toStdString().c_str(), jsonobject.GetAllocator());
        N.PushBack(strVal, jsonobject.GetAllocator());
    }
    jsonobject.AddMember(rapidjson::Value("Members").Move(), N, jsonobject.GetAllocator());
    return true;
}
