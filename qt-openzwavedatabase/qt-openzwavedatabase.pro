QT       -= gui

TEMPLATE = lib

VERSION = 1.0.0

CONFIG += silent file_copies resources_big

!versionAtLeast(QT_VERSION, 5.11.2):error("Use at least Qt version 5.11.2")

ozwconfig.target=qt-openzwavedatabase.rcc
ozwconfig.commands=@echo "Generating qt-openzwavedatabase.rcc" && cp -R ../../open-zwave/config config/ && cd config && $$[QT_INSTALL_BINS]/rcc -project -o ozwconfig.qrc && $$[QT_INSTALL_BINS]/rcc --name="ozwconfig" --root="/config/" ozwconfig.qrc --binary -o ../qt-openzwavedatabase.rcc

ozwrccdb.path=$$[QT_INSTALL_PREFIX]/share/OpenZWave/
ozwrccdb.files+=qt-openzwavedatabase.rcc
ozwrccdb.CONFIG+=no_check_exist

extraclean.commands=rm -rf config qt-openzwavedatabase.rcc
clean.depends=extraclean

!win32 {
    SOURCES += source/qt-openzwavedatabase.cpp
    HEADERS += \
        include/qt-openzwave/qt-openzwavedatabase.h

    INCLUDEPATH += include/

    target.path = /usr/local/lib/
    QMAKE_EXTRA_TARGETS += clean extraclean ozwconfig
    PRE_TARGETDEPS += qt-openzwavedatabase.rcc
    INSTALLS+=target ozwrccdb
    QMAKE_STRIP=@echo
    QMAKE_CXXFLAGS += -g1
    QMAKE_CXXFLAGS += -g1
    QMAKE_LFLAGS += -rdynamic

    macx {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
    }
}

