
QT += widgets remoteobjects testlib

#CONFIG += silent
DEFINES  += remote


include(../qt-openzwave.pri)



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

LIBS += -lresolv
macx {
    QMAKE_CXXFLAGS += -F../qt-openzwave/
    LIBS += -F../qt-openzwave/ -framework qt-openzwave
    ICON = res/ozw_logo.icns
} else {
    LIBS += -L../qt-openzwave/ -lqt-openzwave
    INCLUDEPATH += ../qt-openzwave/qt-openzwave/include/
}
