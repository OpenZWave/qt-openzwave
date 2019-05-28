
QT += widgets remoteobjects testlib

CONFIG += silent
DEFINES  += remote

LIBS += -L../qt-openzwave/ -lqt-openzwave -lresolv
INCLUDEPATH += ../qt-openzwave

include(../qt-openzwave.pri)


macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    ICON = res/ozw_logo.icns
}

unix {
    QMAKE_POST_LINK += "if [ ! -e config ]; then ln -s $$OZW_LIB_PATH/config config; fi"
}

win32 {
    RC_ICONS += res/ozw_logo.ico
}
FORMS += \
    bitsetwidget.ui \
    mainwindow.ui \
    startup.ui

SOURCES = main.cpp \
    bitsetwidget.cpp \
    mainwindow.cpp \
    qtozw_itemdelegate.cpp \
    startup.cpp

HEADERS += \
    bitsetwidget.h \
    mainwindow.h \
    qtozw_itemdelegate.h \
    startup.h

DISTFILES +=

RESOURCES += \
    simpleclient.qrc
