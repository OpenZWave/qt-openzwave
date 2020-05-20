QT       -= gui

TEMPLATE = lib

VERSION = 1.0.0

CONFIG += silent file_copies resources_big force_debug_info

!versionAtLeast(QT_VERSION, 5.12.0):error("Use at least Qt version 5.12.0")

include(../qt-openzwave.pri)

win32-msvc* {
CONFIG += dll
}

DEFINES += QTOZWDATABASE_SHARED_LIBRARY

win32:QMAKE_DEL_DIR  = rmdir /s /q
win32:QMAKE_DEL_FILE = del /q

ozwconfig.target=qt-openzwavedatabase.rcc
ozwconfig.commands+=echo "Generating qt-openzwavedatabase.rcc:"
ozwconfig.commands+=&& $(DEL_FILE) $$shell_path($$OUT_PWD/qt-openzwavedatabase.rcc) 
ozwconfig.commands+=&& $(COPY_DIR) $$shell_path($$OZW_DATABASE_PATH) $$shell_path($$OUT_PWD/config/) 
ozwconfig.commands+=&& cd $$shell_path($$OUT_PWD/config/) 
ozwconfig.commands+=&& $$[QT_INSTALL_BINS]/rcc -project -o $$shell_path($$OUT_PWD/config/ozwconfig.qrc)
ozwconfig.commands+=&& $$[QT_INSTALL_BINS]/rcc --name="ozwconfig" --root="/config/" $$shell_path($$OUT_PWD/config/ozwconfig.qrc) --binary -o $$shell_path($$OUT_PWD/qt-openzwavedatabase.rcc)
ozwconfig.commands+=&& echo "Done"

ozwrccdb.files+=qt-openzwavedatabase.rcc
ozwrccdb.CONFIG+=no_check_exist

extraclean.commands=-$(DEL_FILE) qt-openzwavedatabase.rcc
extraclean.commands+=&& $(DEL_DIR) config
clean.depends=extraclean


SOURCES += source/qt-openzwavedatabase.cpp

PUBLIC_HEADERS.files += include/qt-openzwave/qt-openzwavedatabase.h

HEADERS += $$PUBLIC_HEADERS.files

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
    PUBLIC_HEADERS.path = $$[QT_INSTALL_HEADERS]/qt-openzwave/
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
QMAKE_CXXFLAGS += -g1
QMAKE_LFLAGS += -rdynamic

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

