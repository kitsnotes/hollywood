cmake_minimum_required(VERSION 3.4)

include(FindPkgConfig)
include(CheckIncludeFiles)

project(Horizon
        LANGUAGES C CXX
        VERSION 0.9.7)


add_definitions(-DVERSTR="${PROJECT_VERSION}")


if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-function
                      )
  add_link_options(-Wl,--allow-shlib-undefined)
endif()

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()


option(BUILD_TOOLS "Enable building of tools (Validator, Simulator, etc)" ON)
option(BUILD_UI "Enable user interface" ON)


## Code Coverage stuff ##
option(COVERAGE "Build for code coverage tests (slow)" OFF)
IF(COVERAGE)
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} --coverage")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
    SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
ENDIF(COVERAGE)


## Valgrind stuff ##
find_program(VALGRIND_EXECUTABLE NAMES valgrind)
IF(VALGRIND_EXECUTABLE)
option(VALGRIND "Run Valgrind during test phase" OFF)
ENDIF(VALGRIND_EXECUTABLE)


## Firmware stuff ##
option(UNSUPPORTED_NONFREE_FIRMWARE "Support loading and installation of non-libre firmware (DANGEROUS)" OFF)
mark_as_advanced(FORCE UNSUPPORTED_NONFREE_FIRMWARE)

IF(UNSUPPORTED_NONFREE_FIRMWARE)
    add_definitions(-DNON_LIBRE_FIRMWARE)
ENDIF(UNSUPPORTED_NONFREE_FIRMWARE)


## Installation Environment support stuff ##
option(INSTALL "Build Installation Environment support (Linux only)" ON)

check_include_files(linux/wireless.h HAVE_LINUX_WIRELESS_H)
IF(NOT HAVE_LINUX_WIRELESS_H)
SET(INSTALL OFF)
ENDIF(NOT HAVE_LINUX_WIRELESS_H)


include_directories(.)


## Test stuff ##
find_program(RSPEC_EXECUTABLE NAMES rspec)
enable_testing()


IF(INSTALL)
    add_definitions(-DHAS_INSTALL_ENV)
    pkg_check_modules(BLKID REQUIRED blkid)
    pkg_check_modules(LIBUDEV REQUIRED libudev)
    pkg_check_modules(PARTED REQUIRED libparted)
    set(BUILD_SHARED_LIBS ON)
    option(BUILD_IMAGE "Enable building the image creation tools" ON)

    add_subdirectory(diskman)
    add_subdirectory(executor)
ELSE(INSTALL)
    set(BUILD_SHARED_LIBS OFF)
ENDIF(INSTALL)


   add_definitions(-DFS_IS_STDCXX)


add_subdirectory(hscript)
IF(BUILD_TOOLS)
    add_subdirectory(tools)
ENDIF(BUILD_TOOLS)
IF(BUILD_UI)
    add_subdirectory(ui)
ENDIF(BUILD_UI)
IF(INSTALL)
    add_subdirectory(fetch)
    add_subdirectory(owner)
    IF(BUILD_IMAGE)
        add_subdirectory(image)
    ENDIF(BUILD_IMAGE)
ENDIF(INSTALL)


IF(COVERAGE)
    find_program(LCOV_EXECUTABLE NAMES lcov)

    IF(LCOV_EXECUTABLE)
        add_custom_command(OUTPUT horizon.coverage
            COMMAND lcov ARGS --exclude '/usr/include/boost/*' --exclude '/usr/include/c++/*' --exclude '*/3rdparty/*' --capture --directory ${CMAKE_BINARY_DIR} --output-file horizon.coverage)
        add_custom_target(lcov_report DEPENDS horizon.coverage)
        add_custom_command(OUTPUT cov_html
            COMMAND genhtml ARGS horizon.coverage --output-directory cov_html)
        add_custom_target(lcov_html DEPENDS cov_html)
    ENDIF(LCOV_EXECUTABLE)
ENDIF(COVERAGE)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/devel/image.yang DESTINATION share/doc/horizon)
