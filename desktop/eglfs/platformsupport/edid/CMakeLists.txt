liri_add_module(EdidSupport
    DESCRIPTION
        "EDID parser for EGL device integration"
    SOURCES
        qedidparser.cpp
        qedidparser_p.h
        qedidvendortable_p.h
    PRIVATE_HEADERS
        qedidparser_p.h
        qedidvendortable_p.h
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    LIBRARIES
        Qt5::Core
        Qt5::CorePrivate
    NO_CMAKE
    NO_PKGCONFIG
    STATIC
)

liri_finalize_module(EdidSupport)
