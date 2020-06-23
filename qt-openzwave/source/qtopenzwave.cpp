#include "qt-openzwave/qtopenzwave.h"
#include "qt-openzwave/qtozwmanager.h"
#include "qt-openzwave/qtozw_pods.h"
#include "qt-openzwave/qtozwvalueidmodel.h"
#include "qtozw_logging.h"


Q_LOGGING_CATEGORY(manager, "ozw.manager");
Q_LOGGING_CATEGORY(valueModel, "ozw.values");
Q_LOGGING_CATEGORY(nodeModel, "ozw.nodes");
Q_LOGGING_CATEGORY(associationModel, "ozw.assocations");
Q_LOGGING_CATEGORY(notifications, "ozw.notifications");
Q_LOGGING_CATEGORY(logModel, "ozw.logging");
Q_LOGGING_CATEGORY(libopenzwave, "ozw.library");
Q_LOGGING_CATEGORY(qtrowebsocket, "ozw.remote");
Q_LOGGING_CATEGORY(options, "ozw.options");


QTOpenZwave::QTOpenZwave
(
    QObject *parent,
    QString ConfigPath,
    QString UserPath
) :
    QObject (parent),
    m_manager(nullptr),
    m_ozwconfigpath(ConfigPath),
    m_ozwuserpath(UserPath)
{
    //qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<QTOZW_ValueIDList>("QTOZW_ValueIDList");
    qRegisterMetaTypeStreamOperators<QTOZW_ValueIDList>("QTOZW_ValueIDList");
    qRegisterMetaType<QTOZW_ValueIDBitSet>("QTOZW_ValueIDBitSet");
    qRegisterMetaTypeStreamOperators<QTOZW_ValueIDBitSet>("QTOZW_ValueIDBitSet");

    qRegisterMetaType<NodeStatistics>("NodeStatistics");
    qRegisterMetaTypeStreamOperators<NodeStatistics>("NodeStatistics");

    qRegisterMetaType<OptionList>("OptionList");
    qRegisterMetaTypeStreamOperators<OptionList>("OptionList");

    qRegisterMetaType<NotificationTypes::QTOZW_Notification_Code>("NotificationTypes::QTOZW_Notification_Code");
    qRegisterMetaTypeStreamOperators<NotificationTypes::QTOZW_Notification_Code>("NotificationTypes::QTOZW_Notification_Code");

    qRegisterMetaType<NotificationTypes::QTOZW_Notification_User>("NotificationTypes::QTOZW_Notification_User>");
    qRegisterMetaTypeStreamOperators<NotificationTypes::QTOZW_Notification_User>("NotificationTypes::QTOZW_Notification_User>");

    qRegisterMetaType<NotificationTypes::QTOZW_Notification_Controller_Error>("NotificationTypes::QTOZW_Notification_Controller_Error");
    qRegisterMetaTypeStreamOperators<NotificationTypes::QTOZW_Notification_Controller_Error>("NotificationTypes::QTOZW_Notification_Controller_Error");

    qRegisterMetaType<NotificationTypes::QTOZW_Notification_Controller_State>("NotificationTypes::QTOZW_Notification_Controller_State");
    qRegisterMetaTypeStreamOperators<NotificationTypes::QTOZW_Notification_Controller_State>("NotificationTypes::QTOZW_Notification_Controller_State");

    qRegisterMetaType<NotificationTypes::QTOZW_Notification_Controller_Cmd>("NotificationTypes::QTOZW_Notification_Controller_Cmd");
    qRegisterMetaTypeStreamOperators<NotificationTypes::QTOZW_Notification_Controller_Cmd>("NotificationTypes::QTOZW_Notification_Controller_Cmd");


//    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t}] %{category} %{file}:%{line}:%{function}: %{message}");
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t}] [%{category}] [%{type}]: %{message} %{if-fatal} %{backtrace [depth=10]} %{endif}");
}

QTOZWManager *QTOpenZwave::GetManager
(
)
{
    if (m_manager == nullptr) {
        this->m_manager = new QTOZWManager(this);
        this->m_manager->setOZWConfigPath(m_ozwconfigpath);
        this->m_manager->setOZWUserPath(m_ozwuserpath);
    }
    return this->m_manager;
}

#define VAL(str) #str
#define TOSTRING(str) VAL(str)

QString QTOpenZwave::getVersion() {
    return TOSTRING(LIB_VERSION);
}
