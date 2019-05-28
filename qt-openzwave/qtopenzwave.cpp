#include "qtopenzwave.h"
#include "qtozw_logging.h"


Q_LOGGING_CATEGORY(manager, "ozw.manager");
Q_LOGGING_CATEGORY(valueModel, "ozw.values");
Q_LOGGING_CATEGORY(nodeModel, "ozw.nodes");
Q_LOGGING_CATEGORY(associationModel, "ozw.assocations");
Q_LOGGING_CATEGORY(notifications, "ozw.notifications");


QTOpenZwave::QTOpenZwave
(
    QObject *parent
) :
    QObject (parent),
    m_manager(nullptr)
{
    //qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<QTOZW_ValueIDList>("QTOZW_ValueIDList");
    qRegisterMetaTypeStreamOperators<QTOZW_ValueIDList>("QTOZW_ValueIDList");
    qRegisterMetaType<QTOZW_ValueIDBitSet>("QTOZW_ValueIDBitSet");
    qRegisterMetaTypeStreamOperators<QTOZW_ValueIDBitSet>("QTOZW_ValueIDBitSet");

    qSetMessagePattern("%{category} %{message}");
}

QTOZWManager *QTOpenZwave::GetManager
(
)
{
    if (m_manager == nullptr) {
        this->m_manager = new QTOZWManager(this);
    }
    return this->m_manager;
}
