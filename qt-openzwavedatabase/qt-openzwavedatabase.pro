QT       -= gui

TEMPLATE = lib

VERSION = 1.0.0

CONFIG += silent file_copies resources_big

!versionAtLeast(QT_VERSION, 5.11.2):error("Use at least Qt version 5.11.2")

ozwconfig.target=qt-openzwavedatabase.rcc
ozwconfig.commands=cp -R ../../open-zwave/config config/ && cd config && $$[QT_INSTALL_BINS]/rcc -project -o ozwconfig.qrc && $$[QT_INSTALL_BINS]/rcc --name="ozwconfig" --root="/config/" ozwconfig.qrc --binary -o ../qt-openzwavedatabase.rcc
ozwconfigdb.path=$$[QT_INSTALL_PREFIX]/share/OpenZWave/
ozwconfigdb.files=qt-openzwavedatabase.rcc

!win32 {
    QMAKE_EXTRA_TARGETS += ozwconfig
    PRE_TARGETDEPS += qt-openzwavedatabase.rcc
    SOURCES += source/qt-openzwavedatabase.cpp
    INSTALLS += ozwconfigdb
    macx {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
    }


    HEADERS += \
        include/qt-openzwave/qt-openzwavedatabase.h

    INCLUDEPATH += include/
    QMAKE_CXXFLAGS += -g1
    target.path = /usr/local/lib/
    INSTALLS += target
    QMAKE_CXXFLAGS += -g1
    QMAKE_LFLAGS += -rdynamic
}

