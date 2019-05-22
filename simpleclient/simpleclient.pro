QT += widgets remoteobjects websockets

SOURCES = main.cpp \
    mainwindow.cpp \
    qtozw_itemdelegate.cpp \
    startup.cpp

#CONFIG   -= app_bundle
#CONFIG   += static
CONFIG += silent
DEFINES  += remote

LIBS += -L../qt-openzwave/ -lqt-openzwave -L../open-zwave/ -lopenzwave -lresolv
INCLUDEPATH += ../qt-openzwave ../open-zwave/cpp/src

requires(qtConfig(treeview))

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    ICON = res/ozw_logo.icns
}

win32 {
    RC_ICONS += res/ozw_logo.ico
}
FORMS += \
    mainwindow.ui \
    startup.ui

HEADERS += \
    mainwindow.h \
    qtozw_itemdelegate.h \
    startup.h

DISTFILES +=

RESOURCES += \
    simpleclient.qrc
