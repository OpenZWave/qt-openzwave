QT       -= gui

TEMPLATE = lib

VERSION = 1.0.0

CONFIG += silent file_copies resources_big

!versionAtLeast(QT_VERSION, 5.12.0):error("Use at least Qt version 5.12.0")

include(../qt-openzwave.pri)


ozwconfig.target=qt-openzwavedatabase.rcc
ozwconfig.commands+=@echo "Generating qt-openzwavedatabase.rcc:"
ozwconfig.commands+=&& $(DEL_FILE) qt-openzwavedatabase.rcc 
ozwconfig.commands+=&& $(COPY_DIR) $$OZW_DATABASE_PATH config/ 
ozwconfig.commands+=&& cd config
ozwconfig.commands+=&& $$[QT_INSTALL_BINS]/rcc -project -o ozwconfig.qrc
ozwconfig.commands+=&& $$[QT_INSTALL_BINS]/rcc --name="ozwconfig" --root="/config/" ozwconfig.qrc --binary -o ../qt-openzwavedatabase.rcc
ozwconfig.commands+=&& echo "Done"

ozwrccdb.files+=qt-openzwavedatabase.rcc
ozwrccdb.CONFIG+=no_check_exist

extraclean.commands=rm -rf config qt-openzwavedatabase.rcc
clean.depends=extraclean

!win32 {
    SOURCES += source/qt-openzwavedatabase.cpp
    HEADERS += \
        include/qt-openzwave/qt-openzwavedatabase.h

    INCLUDEPATH += include/
    isEmpty(PREFIX) {
	PREFIX=/usr/local/
    }	
    isEmpty(LIBDIR) {
	    target.path = $$[QT_INSTALL_LIBS]
    } else {
	target.path = $$PREFIX/$$LIBDIR
    }	
    isEmpty(INCDIR) {
	    PUBLIC_HEADERS.path = $$[QT_INSTALL_HEADERS]/$$TARGET/
    } else {
	PUBLIC_HEADERS.path = $$PREFIX/$$INCDIR
    }
    isEmpty(DATADIR) {
	ozwrccdb.path=$$[QT_INSTALL_DATA]
    } else { 
	ozwrccdb.path = $$PREFIX/share/$$TARGET
    }
    message("Final Installation Directories:")
    message("    LibDir: $$target.path")
    message("    IncDir: $$PUBLIC_HEADERS.path")
    message("    DataDir: $$ozwrccdb.path")
    INSTALLS += target
    INSTALLS += PUBLIC_HEADERS
    INSTALLS += ozwrccdb
    QMAKE_EXTRA_TARGETS += clean extraclean ozwconfig
    PRE_TARGETDEPS += qt-openzwavedatabase.rcc
    QMAKE_STRIP=@echo
    QMAKE_CXXFLAGS += -g1 -Wno-deprecated-copy
    QMAKE_LFLAGS += -rdynamic

    macx {
        QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
    }
}

