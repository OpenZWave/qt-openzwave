QT       -= gui

TEMPLATE = lib

VERSION = 1.0.0

CONFIG += silent file_copies resources_big

!versionAtLeast(QT_VERSION, 5.11.2):error("Use at least Qt version 5.11.2")

ozwconfig.target=config/qrc_ozwconfig.cpp
ozwconfig.commands=cp -R ../../open-zwave/config config/ && cd config && $$[QT_INSTALL_BINS]/rcc -project -o ozwconfig.qrc && $$[QT_INSTALL_BINS]/rcc --name="ozwconfig" --root="/config/" ozwconfig.qrc -o qrc_ozwconfig.cpp

!win32 {
    QMAKE_EXTRA_TARGETS += ozwconfig
    PRE_TARGETDEPS += config/qrc_ozwconfig.cpp
    SOURCES += config/qrc_ozwconfig.cpp \
           source/qt-openzwavedatabase.cpp

    macx {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
    }


    HEADERS += \
        include/qt-openzwave/qt-openzwavedatabase.h

    INCLUDEPATH += include/
    QMAKE_CXXFLAGS += -g1
}
