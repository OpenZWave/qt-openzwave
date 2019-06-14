
QT += widgets remoteobjects testlib

CONFIG += silent
DEFINES  += remote

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

RESOURCES += \
    simpleclient.qrc

LIBS += -lresolv
macx {
    QMAKE_CXXFLAGS += -F../qt-openzwave/
    LIBS += -F../qt-openzwave/ -framework qt-openzwave
    ICON = res/ozw_logo.icns
    BUNDLE.files = ../qt-openzwave/qt-openzwave.framework/
    BUNDLE.path = Contents/Frameworks/qt-openzwave.framework/
    LIBOZW.files = ../../open-zwave/libopenzwave-1.6.dylib
    LIBOZW.path = Contents/Frameworks/
    QMAKE_BUNDLE_DATA += BUNDLE LIBOZW
    INCLUDEPATH += ../qt-openzwave/qt-openzwave/include/
} else {
    LIBS += -L../qt-openzwave/ -lqt-openzwave -L../../open-zwave -lopenzwave
    INCLUDEPATH += ../qt-openzwave/include/
}
include(../qt-openzwave.pri)
