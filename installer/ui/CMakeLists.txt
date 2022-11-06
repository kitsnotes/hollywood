find_package(Qt6 COMPONENTS Network Widgets Svg)

IF(Qt6_FOUND)
    option(BUILD_QT "Build the Qt user interface" ON)
    IF(BUILD_QT)
        add_subdirectory(hollywood)
    ENDIF(BUILD_QT)
ENDIF(Qt6_FOUND)
