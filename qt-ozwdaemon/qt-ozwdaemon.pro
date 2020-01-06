QT -= gui

QT += remoteobjects

TARGET = ../ozwdaemon

VERSION = 0.1.0

CONFIG += c++11 console link_pkgconfig silent force_debug_info
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_MESSAGELOGCONTEXT APP_VERSION=$$VERSION

qtHaveModule(mqtt) {
	PKGCONFIG += RapidJSON
	QT += mqtt
	DEFINES += HAVE_MQTT
	SOURCES += mqttpublisher.cpp \
			mqttNodes.cpp \
			mqttValues.cpp \
			mqttAssociations.cpp \
			qtrj.cpp \
        	mqttcommands/mqttcommands.cpp \
	        mqttcommands/ping.cpp \
	        mqttcommands/open.cpp \
			mqttcommands/close.cpp \
        	mqttcommands/refreshnodeinfo.cpp \
	        mqttcommands/requestNodeState.cpp \
        	mqttcommands/requestNodeDynamic.cpp \
	        mqttcommands/requestConfigParam.cpp \
        	mqttcommands/requestAllConfigParam.cpp \
	        mqttcommands/softResetController.cpp \
        	mqttcommands/hardResetController.cpp \
	        mqttcommands/cancelControllerCommand.cpp \
        	mqttcommands/testNetworkNode.cpp \
	        mqttcommands/testNetwork.cpp \
        	mqttcommands/healNetworkNode.cpp \
	        mqttcommands/healNetwork.cpp \
        	mqttcommands/addNode.cpp \
        	mqttcommands/removeNode.cpp \
	        mqttcommands/removeFailedNode.cpp \
        	mqttcommands/hasNodeFailed.cpp \
	        mqttcommands/requestNodeNeighborUpdate.cpp \
        	mqttcommands/assignReturnRoute.cpp \
	        mqttcommands/deleteAllReturnRoute.cpp \
        	mqttcommands/sendNodeInformation.cpp \
	        mqttcommands/replaceFailedNode.cpp \
        	mqttcommands/requestNetworkUpdate.cpp \
	        mqttcommands/IsNodeFailed.cpp \
        	mqttcommands/checkLatestConfigFileRevision.cpp \
	        mqttcommands/checkLatestMFSRevision.cpp \
        	mqttcommands/downloadLatestConfigFileRevision.cpp \
	        mqttcommands/downloadLatestMFSRevision.cpp \
			mqttcommands/setValue.cpp \
			mqttcommands/setPollInterval.cpp \
			mqttcommands/getPollINterval.cpp \
			mqttcommands/syncroniseNodeNeighbors.cpp \
			mqttcommands/refreshValue.cpp

	HEADERS += mqttpublisher.h \
			qtrj.h \
			mqttNodes.h \
			mqttValues.h \
			mqttAssociations.h \
			mqttcommands/mqttcommands.h \
			mqttcommands/ping.h \
			mqttcommands/open.h \
			mqttcommands/close.h \
    		mqttcommands/refreshnodeinfo.h \
    		mqttcommands/requestNodeState.h \
    		mqttcommands/requestNodeDynamic.h \
    		mqttcommands/requestConfigParam.h \
    		mqttcommands/requestAllConfigParam.h \
    		mqttcommands/softResetController.h \
    		mqttcommands/hardResetController.h \
    		mqttcommands/cancelControllerCommand.h \
    		mqttcommands/testNetworkNode.h \
    		mqttcommands/testNetwork.h \
    		mqttcommands/healNetworkNode.h \
    		mqttcommands/healNetwork.h \
    		mqttcommands/addNode.h \
    		mqttcommands/removeNode.h \
    		mqttcommands/removeFailedNode.h \
    		mqttcommands/hasNodeFailed.h \
    		mqttcommands/requestNodeNeighborUpdate.h \
 			mqttcommands/assignReturnRoute.h \
    		mqttcommands/deleteAllReturnRoute.h \
    		mqttcommands/sendNodeInformation.h \
    		mqttcommands/replaceFailedNode.h \
    		mqttcommands/requestNetworkUpdate.h \
    		mqttcommands/IsNodeFailed.h \
    		mqttcommands/checkLatestConfigFileRevision.h \
    		mqttcommands/CheckLatestMFSRevision.h \
    		mqttcommands/downloadLatestConfigFileRevision.h \
    		mqttcommands/downloadLatestMFSRevision.h \
			mqttcommands/setValue.h \
			mqttcommands/setPollInterval.h \
			mqttcommands/getPollInterval.h\
			mqttcommands/syncroniseNodeNeighbors.h \
			mqttcommands/refreshValue.h

} else {
	warning("MQTT Qt Module Not Found. Not Building MQTT Client Capabilities")
}


SOURCES += main.cpp \
        qtozwdaemon.cpp


HEADERS += \
    qtozwdaemon.h \


include(../qt-openzwave.pri)

INCLUDEPATH += ../qt-openzwave/include/


BreakPad {
    exists( $$top_srcdir/../breakpad/src/src/client/linux/libbreakpad_client.a) {
        INCLUDEPATH += $$top_srcdir/../breakpad/src/src/
		SOURCES += $$top_srcdir/../breakpad/src/src/common/linux/http_upload.cc
        LIBS += $$top_srcdir/../breakpad/src/src/client/linux/libbreakpad_client.a -ldl
        DEFINES += BP_LINUX
        message("Building with BreakPad");
    } else {
		error("Can't find BreakPad Library");
	}
}



unix {
	# Default rules for deployment.
	target.path = /usr/local/bin
	INSTALLS += target
	PKGCONFIG += libunwind libcurl
    LIBS += -lresolv -L../qt-openzwave/ -lqt-openzwave -L../qt-openzwavedatabase/ -lqt-openzwavedatabase
    INCLUDEPATH += ../qt-openzwavedatabase/include/
    QMAKE_CXXFLAGS += -g
    QMAKE_CFLAGS += -g
    QMAKE_LFLAGS += -rdynamic
    QMAKE_STRIP = echo
}
win32 {
    LIBS += -lDnsapi -L../qt-openzwave/$$BUILDTYPE/ -lqt-openzwave1
}

macx {
    ICON = res/ozw_logo.icns
    QMAKE_POST_LINK=$$top_srcdir/updaterpath.sh $(TARGET)
}

QMAKE_CFLAGS_RELEASE -= -O
QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
