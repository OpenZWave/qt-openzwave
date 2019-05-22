top_srcdir=$$PWD
top_builddir=$$shadowed($$PWD)

unix {
    isEmpty(OZW_LIB_PATH) {
        QT_CONFIG -= no-pkg-config
        CONFIG += link_pkgconfig
        packagesExist(libopenzwave) {
           message("Using Distribution copy of OpenZWave")
           PKGCONFIG += openzwave
        } else {
            exists( $$top_srcdir/../open-zwave/cpp/src/) {
                message("Found Openzwave in $$absolute_path($$top_srcdir/../open-zwave/cpp/src)")
                OZW_LIB_PATH = $$absolute_path($$top_srcdir/../open-zwave/)
                INCLUDEPATH += $$absolute_path($$top_srcdir/../open-zwave/cpp/src/)
                LIBS += -L$$absolute_path($$top_srcdir/../open-zwave/) -lopenzwave
            } else {
                error("Can't Find a copy of OpenZwave")
            }
        }
    } else {
        exists($$OZW_LIB_PATH/cpp/src/) {
            INCLUDEPATH += $$absolute_path($$OZW_LIB_PATH/cpp/src/)
            LIBS += -L$$absolute_path($$OZW_LIB_PATH) -lopenzwave
            message("Using OZW from Path Supplied in OZW_LIB_PATH varible: $$OZW_LIB_PATH" )
        } else {
            error("Unable to find a copy of OZW in $$OZW_LIB_PATH")
        }
    }

}
