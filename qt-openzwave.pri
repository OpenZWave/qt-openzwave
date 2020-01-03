top_srcdir=$$PWD
top_builddir=$$shadowed($$PWD)

mac {
    PKG_CONFIG = /usr/local/bin/pkg-config
}

unix {
    system("$$PKG_CONFIG --exists openzwave"): USE_PKGCFG = TRUE
    isEmpty(OZW_LIB_PATH) {
        QT_CONFIG -= no-pkg-config
        CONFIG += link_pkgconfig
        if (USE_PKGCFG) {
           PKGCONFIG += openzwave
           message("Using Distribution copy of OZW")
        } else {
            exists( $$top_srcdir/../open-zwave/cpp/src/) {
                message("Found OZW in $$absolute_path($$top_srcdir/../open-zwave/cpp/src)")
                OZW_LIB_PATH = $$absolute_path($$top_srcdir/../open-zwave/)
                INCLUDEPATH += $$absolute_path($$top_srcdir/../open-zwave/cpp/src/)/
                LIBS += -L$$absolute_path($$top_srcdir/../open-zwave/) -lopenzwave
            } else {
                error("Can't Find a copy of OpenZwave")
            }
        }
    } else {
        exists($$OZW_LIB_PATH/cpp/src/) {
            INCLUDEPATH += $$absolute_path($$OZW_LIB_PATH/cpp/src/)/
            LIBS += -L$$absolute_path($$OZW_LIB_PATH) -lopenzwave
            message("Using OZW from Path Supplied in OZW_LIB_PATH varible: $$OZW_LIB_PATH" )
        } else {
            error("Unable to find a copy of OZW in $$OZW_LIB_PATH")
        }
    }

}

win32 {
    CONFIG(debug, debug|release) {
        BUILDTYPE = debug
    } else {
        BUILDTYPE = release
    }
    message(Checking for $$BUILDTYPE build of OZW)
    exists( $$top_srcdir/../open-zwave/cpp/src/) {
        message("Found OZW in $$absolute_path($$top_srcdir/../open-zwave/cpp/src)")
        INCLUDEPATH += $$absolute_path($$top_srcdir/../open-zwave/cpp/src/)/
        equals(BUILDTYPE, "release") {
            exists( $$absolute_path($$top_srcdir/../open-zwave/cpp/build/windows/vs2010/ReleaseDLL/OpenZWave.dll ) ) {
                LIBS += -L$$absolute_path($$top_srcdir/../open-zwave/cpp/build/windows/vs2010/ReleaseDLL) -lopenzwave
                OZW_LIB_PATH = $$absolute_path($$top_srcdir/../open-zwave/cpp/build/windows/vs2010/ReleaseDLL)
            } else {
                error("Can't find a copy of OpenZWave.dll in the ReleaseDLL Directory");
            }
        } else {
            equals(BUILDTYPE, "debug") {
                exists ( $$absolute_path($$top_srcdir/../open-zwave/cpp/build/windows/vs2010/DebugDLL/OpenZWaved.dll )) {
                    LIBS += -L$$absolute_path($$top_srcdir/../open-zwave/cpp/build/windows/vs2010/DebugDLL) -lOpenZWaved
                    OZW_LIB_PATH = $$absolute_path($$top_srcdir/../open-zwave/cpp/build/windows/vs2010/ReleaseDLL)
            } else {
                    error("Can't find a copy of OpenZWaved.dll in the DebugDLL  Directory");
                }
            }
        }
        isEmpty(OZW_LIB_PATH) {
            error("Can't find a copy of OpenZWave with the right builds");
        }
    } else {
        error("Can't Find a copy of OpenZwave")
    }
}
