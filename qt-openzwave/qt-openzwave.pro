#-------------------------------------------------
#
# Project created by QtCreator 2019-05-10T15:07:50
#
#-------------------------------------------------


QT       -= gui
QT       += remoteobjects
#QT       += websockets

TEMPLATE = lib

VERSION = 1.0.0

CONFIG += silent

!versionAtLeast(QT_VERSION, 5.11.2):error("Use at least Qt version 5.11.2")

include(../qt-openzwave.pri)


qtConfig(static) {
 # this is a static build
}

#INCLUDEPATH += $$absolute_path($$top_srcdir/../open-zwave/cpp/src/)/

DEFINES += QTOPENZWAVE_LIBRARY


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        qtopenzwave.cpp \
    qtozwassociations.cpp \
    qtozwmanager.cpp \
    qtozwnotification.cpp \
    qtozwproxymodels.cpp \
    qtozwnodemodel.cpp \
    qtozwvalueidmodel.cpp

HEADERS += \
        qtopenzwave.h \
        qt-openzwave_global.h \  \
        qtozw_logging.h \
        qtozwassociations.h \
        qtozwproxymodels.h \
        qtozwmanager.h \
        qtozwnotification.h \
        qtozwnodemodel.h \
        qtozwvalueidmodel.h


REPC_SOURCE =  qtozwmanager.rep
REPC_REPLICA =  qtozwmanager.rep

unix {
    target.path = /usr/local/lib
    INSTALLS += target
}

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
}

message(" ")
message("Summary:")
message("    OpenZWave Path: $$OZW_LIB_PATH")
message("    Include Path: $$INCLUDEPATH")
message("    Library Path: $$LIBS")
message(" ")
