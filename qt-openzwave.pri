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
