//-----------------------------------------------------------------------------
//
//	qtopenzwave.h
//
//	Main Class For QT-OpenZWave
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
#ifndef QTOPENZWAVE_H
#define QTOPENZWAVE_H

#include <QObject>
#include <QtRemoteObjects>
#include "qt-openzwave_global.h"
#include "qtozwmanager.h"

/**
 * \mainpage QT-OpenZWave
 * \section Introduction Introduction to QT-OpenZWave
 * QT-OpenZWave is an open-source, cross-platform QT Wrapper around the OpenZWave
 * Library designed to allow developers to write QT Based Applications that can
 * interface with a Z-Wave Network.
 * Its Design Closely mimics the main OpenZWave Library in structure, while adopting
 * several QT Technologies such as Signals/Slots and Models to allow
 * quick development of QT based applications.
 * In Addition, it makes use of the QTRemoteObjects module that can expose QT
 * Classes to remote hosts. This allows a QT Application
 * to connect to a host and interact with the Z-Wave Network remotely.
 * <p>
 * We recomend anyone wishing to develop a application using this library has a good
 * grasp of the Z-Wave Specificaitons and Technology and in particular has a good
 * understanding of Nodes, MultiChannel Devices, Association Groups and CommandClasses
 * We also recomend you consult the OpenZWave Library API (and associated documentation)
 * as many of the public methods this library exposes are simple wrappers around the OZW
 * Library
 * <p>
 * Included in the library are two sample applications -
 * SimpleClient is, as the name implies a simple QT GUI Application to expose Nodes,
 * Values, and Association Groups. It also demostrates how to use the QTRemoteObjects
 * feature that allows QT-OpenZWave to connect to remote hosts that have a Z-Wave Dongle or
 * interface.
 * qt-ozwdaemon is console only application that allows remote clients (such as the simpleclient
 * or ozw-admin application) to connect remotely.
 * <p>
 * @image html zwalliance_250x100.jpg
 * <center>OpenZWave is a affiliate member of the Z-Wave Alliance (http://zwavealliance.org/)</center>
 */



/** \brief
 *   Initilizes and Sets up QT-OpenZWave for use.
 *
 *	 \nosubgrouping
 *   The QTOpenZWave class provides the necessary initilization steps to
 *   setup QT-OpenZWave Ready for use. You should create a instance of this
 *   class early in your application.
 *
 *   The Class ensures that the enviroment is ready to start a Copy of OpenZWave
 *   Your Application will then call GetManager() to get a copy of the QTOZW_Manager
 *   class which is the main interface to QT-OpenZWave
 */

class QTOPENZWAVESHARED_EXPORT QTOpenZwave : public QObject
{
    Q_OBJECT
public:
    /** \brief Main Constructor
     * \param parent The Parent Class of this instance
     * \param DBPath if this will be running a local instance (the machine has the attached ZWave Dongle)
     * of OZW, then the OpenZWave Device Database (config directory in the OpenZWave Source) must be present.
     * this is the path to that directory
     * \param UserPath Path where OpenZWave will store/retrive user Specific configuration, such as Options.xml
     * or store the network related files
     */
    QTOpenZwave(QObject *parent = nullptr, QDir DBPath = QDir("./config/"), QDir UserPath = QDir("./config/"));

    /** \brief Obtain a copy of the QTOZW_Manager Instance
     *
     * The QTOZW_Manager class is the main interface for your application. You must only obtain
     * the QTOZW_Manager instance via this call, and never create yourself, to ensure that the enviroment
     * is setup and ready for OpenZWave to start.
     * \returns QTOZWManager a copy of the instance you can use to interact with QT-OpenZWave
     */
    QTOZWManager *GetManager();

private:
    QTOZWManager *m_manager;
    QDir m_ozwdbpath;
    QDir m_ozwuserpath;
};

#endif // QTOPENZWAVE_H
