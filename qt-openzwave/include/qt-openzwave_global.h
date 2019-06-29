//-----------------------------------------------------------------------------
//
//	qt-openzwave_global.h
//
//	Global Definition Header
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

#ifndef QTOPENZWAVE_GLOBAL_H
#define QTOPENZWAVE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTOPENZWAVE_LIBRARY)
#  define QTOPENZWAVESHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTOPENZWAVESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTOPENZWAVE_GLOBAL_H
