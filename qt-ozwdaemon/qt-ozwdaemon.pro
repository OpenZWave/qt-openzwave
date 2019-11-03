QT -= gui

QT += remoteobjects mqtt

TARGET = ../ozwdaemon

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mqttcommands/ping.cpp \
        mqttpublisher.cpp \
        qtozwdaemon.cpp \
        mqttcommands/mqttcommands.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    mqttcommands/ping.h \
    mqttpublisher.h \
    qtozwdaemon.h \
    mqttcommands/mqttcommands.h

include(../qt-openzwave.pri)

INCLUDEPATH += ../qt-openzwave/include/

unix {
    LIBS += -lresolv -L../qt-openzwave/ -lqt-openzwave -L../qt-openzwavedatabase/ -lqt-openzwavedatabase
    INCLUDEPATH += ../qt-openzwavedatabase/include/
}
win32 {
    LIBS += -lDnsapi -L../qt-openzwave/$$BUILDTYPE/ -lqt-openzwave1
}

macx {
    ICON = res/ozw_logo.icns
    QMAKE_POST_LINK=$$top_srcdir/updaterpath.sh $(TARGET)
}


