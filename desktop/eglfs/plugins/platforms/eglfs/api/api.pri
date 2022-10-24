INCLUDEPATH += $$PWD/../../../../platformheaders/
INCLUDEPATH += $$PWD/../../../../libinput/
INCLUDEPATH += $$PWD/../../../../../libhwlogind/
INCLUDEPATH += $$PWD/hollywood/ $$PWD/hollywood/private/

SOURCES +=  $$PWD/qeglfswindow.cpp \
            $$PWD/libinputmanager.cpp \
            $$PWD/qeglfsscreen.cpp \
            $$PWD/qeglfshooks.cpp \
            $$PWD/qeglfsdeviceintegration.cpp \
            $$PWD/qeglfsintegration.cpp \
            $$PWD/qeglfsoffscreenwindow.cpp \
            $$PWD/vthandler.cpp \
            $$PWD/xcursor.c \
            $$PWD/xcursortheme.cpp

HEADERS +=  $$PWD/hollywood/private/qeglfswindow_p.h \
            $$PWD/hollywood/private/libinputmanager_p.h \
            $$PWD/hollywood/private/qeglfsscreen_p.h \
            $$PWD/hollywood/private/qeglfshooks_p.h \
            $$PWD/hollywood/private/qeglfsdeviceintegration_p.h \
            $$PWD/hollywood/private/qeglfsintegration_p.h \
            $$PWD/hollywood/private/qeglfsoffscreenwindow_p.h \
            $$PWD/hollywood/private/qeglfsglobal_p.h \
            $$PWD/hollywood/vthandler.h \
            $$PWD/hollywood/private/vthandler_p.h \
            $$PWD/hollywood/xcursor.h \
            $$PWD/hollywood/private/xcursortheme_p.h

qtConfig(opengl) {
    SOURCES += \
        $$PWD/qeglfscursor.cpp \
        $$PWD/qeglfscontext.cpp
    HEADERS += \
        $$PWD/hollywood/private/qeglfscursor_p.h \
        $$PWD/hollywood/private/qeglfscontext_p.h
}

#qtConfig(vulkan) {
#    SOURCES += \
#        $$PWD/vulkan/qeglfsvulkaninstance.cpp \
#        $$PWD/vulkan/qeglfsvulkanwindow.cpp
#    HEADERS += \
#        $$PWD/vulkan/qeglfsvulkaninstance_p.h \
#        $$PWD/vulkan/qeglfsvulkanwindow_p.h
#}

INCLUDEPATH += $$PWD
