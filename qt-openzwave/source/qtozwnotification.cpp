//-----------------------------------------------------------------------------
//
//	qtozwnotification.cpp
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

#include <QtDebug>
#include "qtozwnotification.h"

OZWNotification *OZWNotification::s_Instance = nullptr;


OZWNotification *OZWNotification::Get()
{
    if (OZWNotification::s_Instance == nullptr) {
        OZWNotification::s_Instance = new OZWNotification();
    }
    return OZWNotification::s_Instance;
}
//-----------------------------------------------------------------------------
// <OZWNotification::processNotification>
// Callback that is triggered when a value, group or node changes
//-----------------------------------------------------------------------------
void OZWNotification::processNotification
(
        OpenZWave::Notification const* _notification,
        void* _context
)
{
    Q_UNUSED(_context);
    //qDebug() << QString(_notification->GetAsString().c_str());
    //qDebug() << _notification;
#if 0
    void valueAdded(quint64 vidKey);
    void valueRemoved(quint64 vidKey);
    void valueChanged(quint64 vidKey);
    void valueRefreshed(quint64 vidKey);
    void valuePollingEnabled(quint64 vidKey);
    void valuePollingDisabled(quint64 vidKey);
#endif

    switch( _notification->GetType() )
        {
        case OpenZWave::Notification::Type_ValueAdded:
        {
            emit Get()->valueAdded(_notification->GetValueID().GetId());
            break;
        }

        case OpenZWave::Notification::Type_ValueRemoved:
        {
            emit Get()->valueRemoved(_notification->GetValueID().GetId());
            break;
        }

        case OpenZWave::Notification::Type_ValueChanged:
        {
            emit Get()->valueChanged(_notification->GetValueID().GetId());
            break;
        }

        case OpenZWave::Notification::Type_ValueRefreshed:
        {
            emit Get()->valueRefreshed(_notification->GetValueID().GetId());
            break;
        }

        case OpenZWave::Notification::Type_PollingDisabled:
        {
            emit Get()->valuePollingDisabled(_notification->GetValueID().GetId());
            break;
        }

        case OpenZWave::Notification::Type_PollingEnabled:
        {
            emit Get()->valuePollingEnabled(_notification->GetValueID().GetId());
            break;
        }

        case OpenZWave::Notification::Type_Group:
        {
            emit Get()->nodeGroupChanged(_notification->GetNodeId(), _notification->GetGroupIdx());
            break;
        }

        case OpenZWave::Notification::Type_NodeNew:
        {
            emit Get()->nodeNew(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_NodeAdded:
        {
            emit Get()->nodeAdded(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_NodeRemoved:
        {
            emit Get()->nodeRemoved(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_NodeReset:
        {
            emit Get()->nodeReset(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_NodeEvent:
        {
            emit Get()->nodeEvent(_notification->GetNodeId(), _notification->GetEvent());
            break;
        }

        case OpenZWave::Notification::Type_DriverReady:
        {
            emit Get()->driverReady(_notification->GetHomeId());
            break;
        }

        case OpenZWave::Notification::Type_DriverFailed:
        {
            emit Get()->driverFailed(_notification->GetHomeId());
            break;
        }

        case OpenZWave::Notification::Type_DriverRemoved:
        {
            emit Get()->driverRemoved(_notification->GetHomeId());
            break;
        }

        case OpenZWave::Notification::Type_DriverReset:
        {
            emit Get()->driverReset(_notification->GetHomeId());
            break;
        }

        case OpenZWave::Notification::Type_ControllerCommand:
        {
            emit Get()->controllerCommand(_notification->GetEvent());
            break;
        }

        case OpenZWave::Notification::Type_AwakeNodesQueried:
        {
            emit Get()->driverAwakeNodesQueried();
            break;
        }

        case OpenZWave::Notification::Type_AllNodesQueried:
        {
            emit Get()->driverAllNodesQueried();
            break;
        }

        case OpenZWave::Notification::Type_EssentialNodeQueriesComplete:
        {
            emit Get()->nodeEssentialNodeQueriesComplete(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_AllNodesQueriedSomeDead:
        {
            emit Get()->driverAllNodesQueriedSomeDead();
            break;
        }

        case OpenZWave::Notification::Type_Notification:
        {
            //emit Get()->ozwNotification(_notification->GetNotification());
            break;
        }

        case OpenZWave::Notification::Type_NodeNaming:
        {
            emit Get()->nodeNaming(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_NodeQueriesComplete:
        {
            emit Get()->nodeQueriesComplete(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_NodeProtocolInfo:
        {
            emit Get()->nodeProtocolInfo(_notification->GetNodeId());
            break;
        }

        case OpenZWave::Notification::Type_UserAlerts:
        {

            break;
        }

        case OpenZWave::Notification::Type_ManufacturerSpecificDBReady:
        {
            emit Get()->manufacturerSpecificDBReady();
            break;
        }

        case OpenZWave::Notification::Type_SceneEvent:
        {
            break;
        }

        case OpenZWave::Notification::Type_CreateButton:
        {
            break;
        }

        case OpenZWave::Notification::Type_DeleteButton:
        {
            break;
        }

        case OpenZWave::Notification::Type_ButtonOn:
        {
            break;
        }

        case OpenZWave::Notification::Type_ButtonOff:
        {
            break;
        }
    }
};

