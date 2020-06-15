//-----------------------------------------------------------------------------
//
//	qtozwnotification.h
//
//	The Notification Callback to Interface OZW and the QT Wrapper
//
//	Copyright (c) 2019 Justin Hammond <Justin@dynam.ac>
//
//	SOFTWARE NOTICE AND LICENSE
//
//	This file is part of QT-OpenZWave.
//
//	OpenZWave is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as published
//	by the Free Software Foundation, either version 3 of the License,
//	or (at your option) any later version.
//
//	OpenZWave is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public License
//	along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------

#ifndef QTOZWNOTIFICATION_H
#define QTOZWNOTIFICATION_H

#include "qt-openzwave/qtopenzwave.h"

#include <QObject>
#include "Notification.h"
#include "Driver.h"

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
    void controllerCommand(quint8 node, quint32 cmd, quint32 state, quint32 error);
    void ozwNotification(quint8 node, quint32 event);
    void ozwUserAlert(quint8 node, quint32 event, quint8 retry);
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
