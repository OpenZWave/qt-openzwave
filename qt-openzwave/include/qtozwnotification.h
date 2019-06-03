#ifndef QTOZWNOTIFICATION_H
#define QTOZWNOTIFICATION_H

#include <QObject>
#include "Notification.h"

class OZWNotification : public QObject {
    Q_OBJECT
public:
    static OZWNotification *Get();
    //-----------------------------------------------------------------------------
    // All our Signals that we emit - one for each type of Notification
    //-----------------------------------------------------------------------------
signals:
    void valueAdded(quint64 vidKey);
    void valueRemoved(quint64 vidKey);
    void valueChanged(quint64 vidKey);
    void valueRefreshed(quint64 vidKey);
    void valuePollingEnabled(quint64 vidKey);
    void valuePollingDisabled(quint64 vidKey);
    void nodeGroupChanged(quint8 node, quint8 group);
    void nodeNew(quint8 node);
    void nodeAdded(quint8 node);
    void nodeRemoved(quint8 node);
    void nodeReset(quint8 node);
    void nodeNaming(quint8 node);
    void nodeEvent(quint8 node, quint8 event);
    void nodeProtocolInfo(quint8 node);
    void nodeEssentialNodeQueriesComplete(quint8 node);
    void nodeQueriesComplete(quint8 node);
    void driverReady(quint32 homeID);
    void driverFailed(quint32 homeID);
    void driverReset(quint32 homeID);
    void driverRemoved(quint32 homeID);
    void driverAllNodesQueriedSomeDead();
    void driverAwakeNodesQueried();
    void driverAllNodesQueried();
    void controllerCommand(quint8 command);
    void ozwNotification(OpenZWave::Notification::NotificationCode event);
    void ozwUserAlert(OpenZWave::Notification::UserAlertNotification event);
    void manufacturerSpecificDBReady();
    //-----------------------------------------------------------------------------
    // <OZWNotification::processNotification>
    // Callback that is triggered when a value, group or node changes
    //-----------------------------------------------------------------------------
public:
    static void processNotification(OpenZWave::Notification const* _notification, void* _context);
private:
    static OZWNotification *s_Instance;

};


#endif // QTOZWNOTIFICATION_H
