
QT += widgets remoteobjects testlib

TARGET = ../simpleclient

#CONFIG += silent
DEFINES  += remote

#unix {
#    QMAKE_POST_LINK += "if [ ! -e config ]; then ln -s $$OZW_LIB_PATH/config config; fi"
#}

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

include(../qt-openzwave.pri)

unix {
    LIBS += -lresolv -L../qt-openzwave/ -lqt-openzwave
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
        qtConfig(static) {
                QMAKE_LFLAGS += -static-libgcc
                LIBS += -lopenzwave -Wl,--allow-multiple-definition
                # this is a static build
        }
}
win32 {
    LIBS += -lDnsapi -L../qt-openzwave/$$BUILDTYPE/ -lqt-openzwave1
}

INCLUDEPATH += ../qt-openzwave/include/

macx {
    ICON = res/ozw_logo.icns
    LIBOZW.files = ../../open-zwave/libopenzwave-1.6.dylib ../qt-openzwave/libqt-openzwave.1.dylib
    LIBOZW.path = Contents/Frameworks/
    QMAKE_BUNDLE_DATA += BUNDLE LIBOZW
    QMAKE_POST_LINK=$$top_srcdir/updaterpath.sh $(TARGET)
}
