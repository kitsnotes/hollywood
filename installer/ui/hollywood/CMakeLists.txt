set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(UI_SOURCES
    horizonwizard.cc
    horizonwizardpage.cc
    horizonhelpwindow.cc
    main.cc
    ${CMAKE_SOURCE_DIR}/3rdparty/Section.cpp
    stepprogresswidget.cc
    subnetbox.cc
    customwifidialog.cc
    mountdialog.cc
    useraccountwidget.cc
    avatardialog.cc
    advancedoptionsdialog.cc
    crypt_sha512.c
    intropage.cc
    inputpage.cc
    partitiondiskpage.cc
    partitionchoicepage.cc
    partitionmanualpage.cc
    partitionmountpage.cc
    datetimepage.cc
    hostnamepage.cc
    bootpage.cc
    rootpwpage.cc
    accountpage.cc

    horizon.qrc)

set(RUN_QT_SOURCES
    horizonhelpwindow.cc
    horizonwizardpage.cc
    stepprogresswidget.cc
    runner/main.cc
    runner/executorwizard.cc
    runner/executepage.cc
    runner/errorpage.cc
    runner/finishedpage.cc

    horizon.qrc)

IF(INSTALL)
    LIST(APPEND UI_SOURCES
        commitpage.cc
        partitionprobe.cc
        partitionpage.cc)
ELSE(INSTALL)
    LIST(APPEND UI_SOURCES writeoutpage.cc)
ENDIF(INSTALL)

IF(UNSUPPORTED_NONFREE_FIRMWARE)
    LIST(APPEND UI_SOURCES firmwarepage.cc)
ENDIF(UNSUPPORTED_NONFREE_FIRMWARE)

link_directories("../../../desktop/build-qt6/output/")
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)
add_executable(hollywood-setup ${UI_SOURCES})
target_link_libraries(hollywood-setup Qt6::Network Qt6::Widgets Qt6::Svg OpenSSL::Crypto)

include_directories("/usr/include/hollywood")

# These entries are for building the main hollywood APK package
# without any elements installed in the root system. do not remove.
include_directories("../../../desktop/libcommdlg")

IF("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set_property(TARGET hollywood-setup PROPERTY CXX_STANDARD 17)
ENDIF()

install(TARGETS hollywood-setup DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/horizon-qt5.1 DESTINATION share/man/man1)

IF(INSTALL)
    pkg_check_modules(XKBFile REQUIRED xkbfile)
    pkg_check_modules(LibCap REQUIRED libcap)
    target_link_libraries(hollywood-setup ${LIBUDEV_LIBRARIES} ${LibCap_LIBRARIES} ${LIBX11_LIBRARIES} ${XKBFile_LIBRARIES} commdlg-1.0 diskman)
    add_executable(hollywood-setup-run ${RUN_QT_SOURCES})
    target_link_libraries(hollywood-setup-run Qt6::Widgets Qt6::Svg commdlg-1.0)
    target_compile_definitions(hollywood-setup-run PRIVATE IN_RUNNER)
    IF("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        set_property(TARGET hollywood-setup-run PROPERTY CXX_STANDARD 17)
    ENDIF()
    install(TARGETS hollywood-setup-run DESTINATION bin)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/horizon-run-qt5.8 DESTINATION share/man/man8)
ENDIF(INSTALL)
