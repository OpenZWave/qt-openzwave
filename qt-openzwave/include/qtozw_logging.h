//-----------------------------------------------------------------------------
//
//	qtozw_logging.h
//
//	Internal QT Logging Functions
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


#ifndef QTOZW_LOGGING_H
#define QTOZW_LOGGING_H
#include "qt-openzwave/qtopenzwave.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(manager);
Q_DECLARE_LOGGING_CATEGORY(valueModel);
Q_DECLARE_LOGGING_CATEGORY(nodeModel);
Q_DECLARE_LOGGING_CATEGORY(logModel);
Q_DECLARE_LOGGING_CATEGORY(associationModel);
Q_DECLARE_LOGGING_CATEGORY(notifications);
Q_DECLARE_LOGGING_CATEGORY(libopenzwave);

#endif // QTOZW_LOGGING_H
